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

/*! \brief Inline function to create engine IDs. */
static inline uint32_t compose_engine_id(uint32_t t, uint32_t i)
{
    return (t << 8) | (i & ((1 << 8) - 1));
}

static inline const char* ram_type_name(const uint32_t ram_type)
{
    switch (ram_type) {
        case NV2080_CTRL_FB_INFO_RAM_TYPE_SDRAM:
            return "SDRAM";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_DDR1:
            return "DDR1";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR2:
            return "SDDR2";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR2:
            return "GDDR2";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR3:
            return "GDDR3";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR4:
            return "GDDR4";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR3:
            return "SDDR3";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5:
            return "GDDR5";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR2:
            return "LPDDR2";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR4:
            return "SDDR4";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR4:
            return "LPDDR4";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_HBM1:
            return "HBM1";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_HBM2:
            return "HBM2";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5X:
            return "GDDR5";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6:
            return "GDDR6";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6X:
            return "GDDR6X";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR5:
            return "LPDDR5";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_HBM3:
            return "HBM3";
        case NV2080_CTRL_FB_INFO_RAM_TYPE_UNKNOWN:
        default:
            return "<unknown>";
    };
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

    mgr->nvidia.vm_uuid = vm_uuid;
    mgr->nvidia.pid = vm_start_info.qemu_pid;
    mgr->nvidia.mdev_id = vm_start_info.mdev_id;

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
    const uint32_t FB_INFO_VALUES[16] = {
    NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE,
    NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT,
    NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE,
    NV2080_CTRL_FB_INFO_INDEX_BUS_WIDTH,
    NV2080_CTRL_FB_INFO_INDEX_RAM_TYPE,
    NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK,
    NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT,
    NV2080_CTRL_FB_INFO_INDEX_FBP_MASK,
    NV2080_CTRL_FB_INFO_INDEX_L2CACHE_SIZE,
    NV2080_CTRL_FB_INFO_INDEX_1TO1_COMPTAG_ENABLED,
    NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT,
    NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED,
    NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED,
    NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED,
    NV2080_CTRL_FB_INFO_INDEX_LTC_MASK,
    NV2080_CTRL_FB_INFO_INDEX_LTS_COUNT
    };

    struct Nv2080CtrlDMAGetInfoParams dma_info = {};
    struct Nv2080CtrlFIFOGetPhysicalChannelCount physical_count = {};
    struct Nv2080GetFBInfoV2 fb_info = {};
    struct Nv0080CtrlGRGetCapsV2 gr_caps = {};
    struct NvA082AllocParams a082_alloc_params = {};
    struct NvA084AllocParams a084_alloc_params = {};

    mgr->nvidia.max_pfn_count = 512 * 1024;
    mgr->nvidia.guest_cache = calloc(mgr->nvidia.max_pfn_count, sizeof(uint64_t));

    memset(mgr->nvidia.guest_cache, -1, mgr->nvidia.max_pfn_count * sizeof(uint64_t));

    mgr->nvidia.fb_length = 256 * 1024 * 1024;

    // magic stuff i do not know yet (TODO: SUPPORT SR-IOV MODE).
    mgr->nvidia.mdev_caps = 0b00000000000000000010000011110101; // | 0x2 to set cuda mode, disabled by default.
    mgr->nvidia.writeable_mdev_caps = 0b00000000000000000000010011100000;

    dma_info.table_size = 1;
    fb_info.table_size = 16;

    for (int i = 0; i < 16; ++i) {
        fb_info.fb_info[i].index = FB_INFO_VALUES[i];
    }

    RM_CTRL(
        mdev_gpu->ctl_fd,
        mdev_gpu->sdev,
        NV2080_GET_DMA_INFO,
        dma_info
    );
    RM_CTRL(
        mdev_gpu->ctl_fd,
        mdev_gpu->sdev,
        NV2080_FIFO_GET_PHYSICAL_CHANNEL_COUNT,
        physical_count
    );
    RM_CTRL(
        mdev_gpu->ctl_fd,
        mdev_gpu->sdev,
        NV2080_GET_FB_INFO_V2,
        fb_info
    );
    RM_CTRL(
        mdev_gpu->ctl_fd,
        mdev_gpu->dev,
        NV0080_GET_GR_CAPS_V2,
        gr_caps
    );

    mgr->nvidia.gid.flags = 0x00000002;

    RM_CTRL(
        mdev_gpu->ctl_fd,
        mdev_gpu->sdev,
        NV2080_GPU_GET_GID_INFO,
        mgr->nvidia.gid
    );

    a084_alloc_params.mdev_dev_id = mdev_gpu->root;
    a084_alloc_params.weird_stuff = 1;
    a084_alloc_params.vm_pid = mgr->nvidia.pid;
    a084_alloc_params.uuid_or_magic = mgr->nvidia.vm_uuid;

    mgr->nvidia.mdev_dev_id = compose_vm_id(
        mgr->nvidia.mdev_id,
        5
    );
    mgr->nvidia.mdev_dev_id2 = compose_vm_id(
        mgr->nvidia.mdev_id,
        6
    );

    a082_alloc_params.mdev_instance_id = mdev_gpu->root;
    a082_alloc_params.vm_id_type = 1;
    a082_alloc_params.vm_id = mgr->nvidia.vm_uuid;
    a082_alloc_params.vm_pid = mgr->nvidia.pid;

    mgr->nvidia.mdev_device = rm_alloc_res(
        mdev_gpu->ctl_fd, mdev_gpu->sdev, mgr->nvidia.mdev_dev_id,
        NVA084_CLASS, &a084_alloc_params
    );

    mgr->nvidia.mdev_device2 = rm_alloc_res(
        mdev_gpu->ctl_fd, mdev_gpu->sdev, mgr->nvidia.mdev_dev_id2,
        NVA082_CLASS, &a082_alloc_params
    );

    mgr->nvidia.max_mfn = ((uint64_t) 1) << (dma_info.dma_info[0].value - 12);
    mgr->nvidia.fifo_channel_count = physical_count.physical_channel_count;
    mgr->nvidia.gpu_fb_mbs = fb_info.fb_info[0].value >> 10;
    mgr->nvidia.fb_info.bank_sizzle_align = fb_info.fb_info[1].value;
    mgr->nvidia.fb_info.fb_len_mbs = fb_info.fb_info[2].value >> 10;
    mgr->nvidia.fb_info.fb_bus_width = fb_info.fb_info[3].value;
    mgr->nvidia.fb_info.ram_type = fb_info.fb_info[4].value;
    mgr->nvidia.fb_info.fbio_mask = fb_info.fb_info[5].value;
    mgr->nvidia.fb_info.fbp_count = fb_info.fb_info[6].value;
    mgr->nvidia.fb_info.fbp_mask = fb_info.fb_info[7].value;
    mgr->nvidia.fb_info.l2_cache_size = fb_info.fb_info[8].value;
    mgr->nvidia.fb_info.comptag_1to1_enabled = fb_info.fb_info[9].value != 0;
    mgr->nvidia.fb_info.page_retirement_enabled = fb_info.fb_info[10].value != 0;
    mgr->nvidia.fb_info.poison_fuse_enabled = fb_info.fb_info[11].value != 0;
    mgr->nvidia.fb_info.fbpa_ecc_vbios_enabled = fb_info.fb_info[12].value != 0;
    mgr->nvidia.fb_info.dynamic_blacklist_enabled = fb_info.fb_info[13].value != 0;
    mgr->nvidia.fb_info.ltc_mask = fb_info.fb_info[14].value;
    mgr->nvidia.fb_info.lts_count = fb_info.fb_info[15].value;

    memcpy(
        mgr->nvidia.gr_caps,
        gr_caps.caps_table,
        NV0080_CTRL_GR_CAPS_TBL_SIZE
    );

    printf(
        "Maximum MFNs supported: %ld\n"
        "Maximum FIFO physical channels supported: %d\n"
        "Size of total GPU framebuffer: %d MBs\n"
        "FB INFO:\n"
        "\tbank_sizzle_align: %d\n"
        "\tfb_len_mbs: %d\n"
        "\tfb_bus_width: %d\n"
        "\tram_type: %s\n"
        "\tfbio_mask: %d\n"
        "\tfbp_count: %d\n"
        "\tfbp_mask: %d\n"
        "\tl2_cache_size: %d\n"
        "\tcomptag_1to1_enabled: %d\n"
        "\tpage_retirement_enabled: %d\n"
        "\tpoison_fuse_enabled: %d\n"
        "\tfbpa_ecc_vbios_enabled: %d\n"
        "\tdynamic_blacklist_enabled: %d\n"
        "\tltc_mask: %d\n"
        "\tlts_count: %d\n",
        mgr->nvidia.max_mfn,
        mgr->nvidia.fifo_channel_count,
        mgr->nvidia.gpu_fb_mbs,
        mgr->nvidia.fb_info.bank_sizzle_align,
        mgr->nvidia.fb_info.fb_len_mbs,
        mgr->nvidia.fb_info.fb_bus_width,
        ram_type_name(mgr->nvidia.fb_info.ram_type),
        mgr->nvidia.fb_info.fbio_mask,
        mgr->nvidia.fb_info.fbp_count,
        mgr->nvidia.fb_info.fbp_mask,
        mgr->nvidia.fb_info.l2_cache_size,
        mgr->nvidia.fb_info.comptag_1to1_enabled,
        mgr->nvidia.fb_info.page_retirement_enabled,
        mgr->nvidia.fb_info.poison_fuse_enabled,
        mgr->nvidia.fb_info.fbpa_ecc_vbios_enabled,
        mgr->nvidia.fb_info.dynamic_blacklist_enabled,
        mgr->nvidia.fb_info.ltc_mask,
        mgr->nvidia.fb_info.lts_count
    );

    mgr->nvidia.mdev_gpu = mdev_gpu;
}
