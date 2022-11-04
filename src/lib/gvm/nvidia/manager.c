/*
 * Copyright (C) 2022 2666680 Ontario Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include <gpu/nvidia/device.h>
#include <gpu/nvidia/resman/api.h>
#include <gpu/nvidia/resman/classes.h>
#include <gpu/nvidia/resman/types.h>

#include <gvm/plugins/base.h>
#include <gvm/nvidia/init.h>
#include <gvm/nvidia/manager.h>

#include <sys/select.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//! RM Event ID.
const uint32_t RM_EVENT = 4;

/*! \brief Inline function to create VM specific Event IDs. */
static inline uint32_t compose_vm_id(uint32_t t, uint32_t i)
{
    return 0xDAE00000 | (t << 8) | i;
}

/*! \brief Code to initialize events for the VM manager.
 *
 * This makes a event handler for the VM manager.
 *
 * \param ctl_fd - Control file descriptor.
 * \param res - Parent resource on the base of the system.
 * \param event - Event id.
 * \param notify - Notification settings.
 * \param flags - Flags to use for the event.
 * \return Returns a file descriptor to associate to the event.
 */
static int event_init(
    int ctl_fd,
    struct NvResource* res,
    uint32_t event,
    uint32_t notify,
    uint32_t flags
)
{
    int fd = nv_open_dev(255);

    rm_alloc_os_event(fd, res->client, res->object);

    struct RmAllocEvent rm_event = {};

    rm_event.parent = res->client;
    rm_event.event_class = 0x00000079;
    rm_event.notify = notify | flags;
    rm_event.event_data = fd;

    rm_alloc_res(ctl_fd, res, event, 0x00000005, &rm_event);

    struct RmSetNotification set_notify = {};

    set_notify.event = notify;
    set_notify.action = 2;

    RM_CTRL(ctl_fd, res, 0x00000501, set_notify);

    return fd;
}

/*! \todo Proper logging.
 */
struct VmMgr init_nv_vm_mgr(struct NvMdev* mgr)
{
    const uint32_t event_start = compose_vm_id(RM_EVENT, 0);
    const uint32_t event_bind = compose_vm_id(RM_EVENT, 1);

    struct VmMgr ret = {};

    for (int i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct NvMdevGpu* gpu = mgr->gpus[i];

        uint32_t persistence = 0;

        RM_CTRL(gpu->ctl_fd, gpu->dev, 0x00800288, persistence);

        if (persistence == 1) {
            persistence = 0;
            RM_CTRL(gpu->ctl_fd, gpu->dev, 0x00800287, persistence);
        }
    }

    printf("Persistence set correctly.\n");

    ret.event_start = event_init(mgr->fd, mgr->res, event_start, 2, 0x10000000);
    ret.event_bind = event_init(mgr->fd, mgr->res, event_bind, 3, 0x10000000);
    ret.root = mgr->res->client;

    printf("Events initialized for start and bind VM.\n");

    return ret;
}

/*! \todo Proper logging.
 */
void handle_nv_vm_start(struct VmMgr* mgr, struct NvMdev* mdev_mgr)
{
    int        n_fds;
    fd_set     read_fds;

    FD_ZERO(&read_fds);

    FD_SET(mgr->event_start, &read_fds);
    FD_SET(mgr->event_bind, &read_fds);

    n_fds = (mgr->event_start > mgr->event_bind ? mgr->event_start : mgr->event_bind) + 1;

    select(n_fds, &read_fds, NULL, NULL, NULL);

    if (FD_ISSET(mgr->event_start, &read_fds)) {
        int ret = fork();
        if (ret == 0) {
            ret = fork();

            if (ret == 0) {
                printf("Got a start request from the NVIDIA kernel module\n");
                start_nv_vm(mgr, mdev_mgr);
            } else {
                exit(0);
            }
        }
    } else if (FD_ISSET(mgr->event_bind, &read_fds)) {
        printf("Got a bind request from the NVIDIA kernel module\n");
    }
}

/*! \todo Proper logging.
 */
void start_nv_vm(struct VmMgr* mgr, struct NvMdev* mdev_mgr)
{
    struct RmVmStartInfo vm_start_info = {};

    RM_CTRL(mdev_mgr->fd, mdev_mgr->res, 0x00000C01, vm_start_info);

    struct UUID uuid = vm_start_info.uuid;
    struct UUID vm_uuid = {};

    vm_uuid.time_low = 0xEE1AEACA;
    vm_uuid.time_mid = 0xFD56;
    vm_uuid.time_hi_and_version = 0x470F;
    vm_uuid.clock_seq_hi_and_reserved = 0xBB;
    vm_uuid.clock_seq_low = 0x85;
    vm_uuid.node[0] = 0x1E;
    vm_uuid.node[1] = 0xF6;
    vm_uuid.node[2] = 0x67;
    vm_uuid.node[3] = 0x54;
    vm_uuid.node[4] = 0x9D;
    vm_uuid.node[5] = 0xE5;

    printf(
        "MDEV UUID: %.8X-%.4X-%.4X-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X\n"
        "Got config \"%s\" for QEMU PID %d\n"
        "Starting VM...\n",
        uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
        uuid.clock_seq_hi_and_reserved, uuid.clock_seq_low,
        uuid.node[0], uuid.node[1], uuid.node[2], uuid.node[3],
        uuid.node[4], uuid.node[5],
        vm_start_info.config,
        vm_start_info.qemu_pid
    );

    struct RmVmNotifyStart notify_start = {};

    notify_start.mdev = uuid;
    notify_start.vm = vm_uuid;
    strcpy(notify_start.name, "GVM VM");

    printf("Opened /dev/nvidia-vgpu%d\n", vm_start_info.mdev_id);

    mgr->mdev_fd = nv_open_mdev(vm_start_info.mdev_id);

    struct VmListener vm_ctrl = nv_plugin_env_setup(vm_start_info.mdev_id);
    struct NvMdevGpu* gpu = mdev_mgr->gpus[0];

    for (int i = 0; i < 32 && mdev_mgr->gpus[i] != NULL; ++i) {
        gpu = mdev_mgr->gpus[0];

        if (gpu->gpu->identifier != vm_start_info.pci_id)
            continue;

        RM_CTRL(gpu->dev_fd, gpu->mdev, NVA081_NOTIFY_START, notify_start);
        printf("Started VM\n");
        break;
    }

    if (gpu != NULL) {
        init_nv_mgr(mgr, gpu);
        nv_plugin_start(&vm_ctrl);
    }
}

/*! \todo Proper logging.
 */
void init_nv_mgr(struct VmMgr* mgr, struct NvMdevGpu* mdev_gpu)
{
    mgr->nvidia.max_pfn_count = 512 * 1024;
    mgr->nvidia.guest_cache = calloc(mgr->nvidia.max_pfn_count, sizeof(uint64_t));

    memset(mgr->nvidia.guest_cache, -1, mgr->nvidia.max_pfn_count * sizeof(uint64_t));

    mgr->nvidia.mdev_gpu = mdev_gpu;
}
