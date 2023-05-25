/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <gpu/nvidia/device.h>
#include <gpu/nvidia/init.h>
#include <gpu/nvidia/resources.h>
#include <gpu/nvidia/resman.h>

#include <gvm/apis.h>
#include <gvm/creator.h>
#include <gvm/nvidia/creator.h>
#include <gvm/nvidia/engines.h>
#include <gvm/nvidia/manager.h>
#include <gvm/nvidia/debug/envy.h>

#include <utils/gpus.h>

#include <class/cl0005.h>
#include <class/cla081.h>

#include <ctrl/ctrl0000/ctrl0000event.h>
#include <ctrl/ctrl0000/ctrl0000vgpu.h>
#include <ctrl/ctrl0080.h>
#include <ctrl/ctrla081.h>

#include <rs_access.h>

#include <nvos.h>
#include <nvtypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>

//! Initialization API.
#define initializer INIT_APIS[Nvidia]

//! Creator API.
#define creator CREATOR_APIS[Nvidia]

//! Handle Magic for event creation.
#define VGPU_HANDLE 0xDAE00000

//! Event for vGPU.
#define VGPU_EVENT 4

/*! \brief Creator device id codebase.*/
static uint32_t manager_ids(
    uint32_t client, uint32_t gpu_id, uint32_t handle
)
{
    return (((client + gpu_id) << 8) | handle);
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
 * \param cmd - Command for the event.
 * \param action - Action for the event.
 * \return Returns a file descriptor to associate to the event.
 */
static int event_init(
    int ctl_fd,
    struct NvResource* res,
    uint32_t event,
    uint32_t notify,
    uint32_t flags,
    uint32_t cmd,
    uint32_t action
)
{
    int fd = nv_open_dev(255);
    NV0005_ALLOC_PARAMETERS rm_event = {};
    NV0000_CTRL_EVENT_SET_NOTIFICATION_PARAMS set_notify = {};

    printf("event: 0x%.8X\nnotify: 0x%.8X\nflags: 0x%.8X\n", event, notify, flags);

    rm_alloc_os_event(fd, res->client, res->object);

    rm_event.hParentClient = res->client;
    rm_event.hClass = NV01_EVENT_OS_EVENT;
    rm_event.notifyIndex = notify | flags;
    rm_event.data = NV_PTR_TO_NvP64(fd);

    rm_alloc_res(ctl_fd, res, event, NV01_EVENT, &rm_event);

    set_notify.event = notify;
    set_notify.action = action;

    RM_CTRL(ctl_fd, res, cmd, set_notify);

    return fd;
}

/*! \brief Initialization Code for NVIDIA.
 *
 * This code initializes the NVIDIA GVM system for manager.
 */
static uint8_t init(void *in)
{
    uint8_t ret = 0;

    struct NvManager *mgr = (struct NvManager*) in;
    struct NvCreator *crt = NULL;
    struct NvInitData *dat = NULL;

    if (mgr == NULL)
        return 1;

    mgr->dat = (struct NvInitData*) calloc(1, sizeof(struct NvInitData));

    dat = mgr->dat;

    if (dat == NULL)
        return 1;

    dat->device_ids = manager_ids;
    dat->share_type = RS_SHARE_TYPE_CLIENT;
    dat->ignore_version_check = mgr->ignore_version_check;

    if (initializer->init(dat)) {
        ret = 1;
        goto failed_init;
    }

    printf("Created manager for NVIDIA.\n");

    mgr->crt = (struct NvCreator*) calloc(1, sizeof(struct NvCreator));

    crt = mgr->crt;

    if (crt == NULL) {
        ret = 1;
        goto failed_init;
    }

    crt->ignore_version_check = mgr->ignore_version_check;
    crt->create_signature = mgr->create_signature;

    if (creator->init(crt)) {
        ret = 1;
        goto failed_creator;
    }

    printf("Initialized creator for the manager.\n");

    struct AttachedGpus *gpus = initializer->get_gpus(mgr->dat);

    if (gpus == NULL) {
        ret = 1;
        goto failed;
    }

    for (uint32_t i = 0; i < gpus->num_gpus; ++i) {
        struct NvGpu *ngpu = (struct NvGpu*) gpus->gpus[i].vendor_info;
        const uint32_t vgpu_config_id = dat->device_ids(
            dat->root->object, gpus->gpus[i].identifier, 3
        );
        NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS query_persist = {};
        NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS modify_persist = {};

        if (ngpu == NULL) continue;

        RM_CTRL(
            dat->ctl_fd,
            ngpu->dev,
            NV0080_CTRL_CMD_GPU_QUERY_SW_STATE_PERSISTENCE,
            query_persist
        );

        if (query_persist.swStatePersistence ==
            NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_DISABLED) {
            modify_persist.newState =
                NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_ENABLED;

            RM_CTRL(
                dat->ctl_fd,
                ngpu->dev,
                NV0080_CTRL_CMD_GPU_MODIFY_SW_STATE_PERSISTENCE,
                modify_persist
            );
        }

        ngpu->engines[VGPU_CONFIG_INDEX] = rm_alloc_res(
            dat->ctl_fd, ngpu->sub_dev, vgpu_config_id, NVA081_VGPU_CONFIG, NULL
        );

        if (ngpu->engines[VGPU_CONFIG_INDEX] == NULL) {
            printf("Failed to create vGPU configuration engine.\n");
            goto failed;
        }
    }

    const uint32_t event_start = VGPU_HANDLE | (VGPU_EVENT << 8) | 0;
    const uint32_t event_bind = VGPU_HANDLE | (VGPU_EVENT << 8) | 1;

    printf("Creating event FDs.\n");

    mgr->event_start = event_init(
        dat->ctl_fd,
        dat->root,
        event_start,
        NV0000_NOTIFIERS_VM_START,
        NV01_EVENT_WITHOUT_EVENT_DATA,
        NV0000_CTRL_CMD_EVENT_SET_NOTIFICATION,
        NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT
    );
    mgr->event_bind = event_init(
        dat->ctl_fd,
        dat->root,
        event_bind,
        NV0000_NOTIFIERS_GPU_BIND_EVENT,
        NV01_EVENT_WITHOUT_EVENT_DATA,
        NV0000_CTRL_CMD_EVENT_SET_NOTIFICATION,
        NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT
    );

    mgr->running = 1;

    return 0;

failed:
    printf("Stopping creator.\n");
    creator->stop(crt);

failed_creator:
    printf("Cleaning creator.\n");
    free(crt);

    mgr->crt = NULL;

    initializer->stop(mgr->dat);

failed_init:
    printf("Cleaning manager.\n");
    free(dat);

    mgr->dat = NULL;

    return ret;
}

/*! \brief Stops the NVIDIA Manager API.
 */
static uint8_t stop(void *in)
{
    struct NvManager *mgr = (struct NvManager*) in;

    if (mgr == NULL || mgr->dat == NULL || mgr->crt == NULL)
        return 1;

    initializer->stop(mgr->dat);
    creator->stop(mgr->crt);

    free(mgr->dat);
    free(mgr->crt);

    mgr->dat = NULL;
    mgr->crt = NULL;

    return 0;
}

/*! \brief Code to check if the Nvidia Manager is running or not.
 */
static uint8_t running(void *in)
{
    struct NvManager *mgr = (struct NvManager*) in;

    if (mgr == NULL)
        return 0;

    return mgr->running;
}

/*! \brief Create mdev forwarding code.
 */
static uint8_t create_mdevs(
    void *in,
    struct MDevRequest *requested, size_t mdev_length,
    struct Gpu *restricted, size_t restrict_length
)
{
    struct NvManager *mgr = (struct NvManager*) in;

    if (mgr == NULL || mgr->crt == NULL)
        return 1;

    return creator->create_mdevs(
        mgr->crt,
        requested, mdev_length,
        restricted, restrict_length
    );
}

/*! \brief Register mdev forwarding code.
 */
static uint8_t register_mdevs(void *in)
{
    struct NvManager *mgr = (struct NvManager*) in;

    if (mgr == NULL || mgr->crt == NULL)
        return 1;

    return creator->register_mdevs(mgr->crt);
}

/*! \brief Starts a VM.
 */
static void start_vm(struct NvManager *mgr)
{
    uint32_t status = 1;
    struct NvInitData *dat = NULL;
    struct AttachedGpus *gpus = NULL;
    struct NvGpu *ngpu = NULL;
    struct UUID vm_uuid = {};
    NV0000_CTRL_VGPU_GET_START_DATA_PARAMS vm_start_data = {};
    NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS start = {};

    if (mgr == NULL || mgr->dat == NULL || mgr->dat->gpus == NULL)
        return;

    dat = mgr->dat;
    gpus = mgr->dat->gpus;

    RM_CTRL(
        dat->ctl_fd,
        dat->root,
        NV0000_CTRL_CMD_VGPU_GET_START_DATA,
        vm_start_data
    );

    printf(
        "Starting vm pid: %d, with configs: %s\n",
        vm_start_data.qemuPid,
        vm_start_data.configParams
    );

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

    memcpy((void*) start.vmUuid, (void*) &vm_uuid, VM_UUID_SIZE);
    memcpy((void*) start.mdevUuid, (void*) vm_start_data.mdevUuid, VM_UUID_SIZE);
    strncpy((char*) start.vmName, "GVM VM", NVA081_VM_NAME_SIZE);

    for (uint32_t i = 0; i < gpus->num_gpus; ++i) {
        if (gpus->gpus[i].identifier == vm_start_data.gpuPciId) {
           ngpu = (struct NvGpu*) gpus->gpus[i].vendor_info;
           break;
        }
    }

    if (ngpu == NULL) {
        printf("Invalid GPU selected.\n");
        return;
    }

    // TODO: NOT IMPLEMENTED YET.
    start.returnStatus = status;

    RM_CTRL(
        ngpu->fd,
        ngpu->engines[VGPU_CONFIG_INDEX],
        NVA081_CTRL_CMD_VGPU_CONFIG_NOTIFY_START,
        start
    );
}

/*! \brief Handles an event.
 */
static uint8_t handle_event(void *in)
{
    int n_fds;
    fd_set read_fds;
    struct NvManager *mgr = (struct NvManager*) in;

    if (mgr == NULL)
        return 1;

    FD_ZERO(&read_fds);

    FD_SET(mgr->event_start, &read_fds);
    FD_SET(mgr->event_bind, &read_fds);

    n_fds = (mgr->event_start > mgr->event_bind ?
             mgr->event_start : mgr->event_bind) + 1;

    select(n_fds, &read_fds, NULL, NULL, NULL);

    if (FD_ISSET(mgr->event_start, &read_fds)) {
        int ret = fork();
        if (ret == 0) {
            ret = fork();

            if (ret == 0) {
                printf("Got a start request from the NVIDIA kernel module\n");
                start_vm(mgr);
            } else {
                exit(0);
            }
        }
    } else if (FD_ISSET(mgr->event_bind, &read_fds)) {
        printf("Got a bind request from the NVIDIA kernel module\n");
    }

    return 0;
}

const struct ManagerAPI nvidia_manager_api = {
.init = init,
.create_mdevs = create_mdevs,
.register_mdevs = register_mdevs,
.handle_event = handle_event,
.stop = stop,
.running = running
};
