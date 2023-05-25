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
#include <gpu/init.h>

#include <class/cl0080.h>
#include <class/cl2080.h>

#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl2080/ctrl2080bus.h>
#include <ctrl/ctrl2080/ctrl2080fb.h>

#include <rs_access.h>

#include <stdlib.h>

/*! \brief Initialization Code for NVIDIA.
 *
 * This code is required to initialize the NVIDIA initialization API.
 *
 * TODO: USE PROPER LOGGING.
 */
static uint8_t init(void *in)
{
    uint8_t ret = 0;
    int fd;
    struct NvInitData *dat = (struct NvInitData*) in;

    if (dat == NULL)
        return 1;

    fd = nv_open_dev(255);

    if (!rm_version_check(fd, dat->ignore_version_check, RM_VERSION)) {
        printf("Incorrect version of RM APIs.\n");
        ret = 1;
    }

    if (!ret) {
        printf("Creating root for NVIDIA.\n");
        dat->ctl_fd = fd;
        dat->root = rm_alloc_res(fd, NULL, 0, 0, NULL);
        printf("Created root for NVIDIA 0x%.8X.\n", dat->root->object);
    }

    printf("Initialized root for NVIDIA.\n");

    return ret;
}

/*! \brief Gets the Used Framebuffer for a NVIDIA GPU.
 */
static uint8_t get_used_fb(void *in, struct Gpu *gpu)
{
    int ret = 0;
    struct NvInitData *dat = (struct NvInitData*) in;

    if (dat == NULL || gpu == NULL)
        ret = 1;

    if (!ret) {
        NV2080_CTRL_FB_GET_INFO_V2_PARAMS fb_info = {};
        struct NvGpu *ngpu = (struct NvGpu*) gpu->vendor_info;

        fb_info.fbInfoListSize = 2;
        fb_info.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE;
        fb_info.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE;

        RM_CTRL(
            dat->ctl_fd, ngpu->sub_dev,
            NV2080_CTRL_CMD_FB_GET_INFO_V2, fb_info
        );

        gpu->free_fb = ((uint64_t) fb_info.fbInfoList[0].data) << 10;
        gpu->total_fb = ((uint64_t) fb_info.fbInfoList[1].data) << 10;
    }

    return ret;
}

/*! \brief Attaches and gets NVIDIA GPUs in the system.
 *
 * TODO: USE PROPER LOGGING.
 */
static struct AttachedGpus *get_gpus(void *in)
{
    struct AttachedGpus *ret = calloc(1, sizeof(struct AttachedGpus));
    struct NvInitData *dat = (struct NvInitData*) in;
    struct NvResource *root = NULL;
    int ctl_fd = -1;
    int i = 0;

    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS probed_ids = {};

    if (dat == NULL)
        goto failed;

    root = dat->root;
    ctl_fd = dat->ctl_fd;

    if (dat->gpus != NULL) {
        free(ret);
        ret = dat->gpus;
        return ret;
    }

    if (root == NULL)
        goto failed;

    if (RM_CTRL(ctl_fd, root, NV0000_CTRL_CMD_GPU_GET_PROBED_IDS, probed_ids) ==
        NULL)
        goto failed;

    for (i = 0;
         i < NV0000_CTRL_GPU_MAX_PROBED_GPUS &&
             probed_ids.gpuIds[i] != NV0000_CTRL_GPU_INVALID_ID;
         ++i) {
        NV0000_CTRL_GPU_ATTACH_IDS_PARAMS attach_ids = {};
        NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS pci_info = {
            .gpuId = probed_ids.gpuIds[i]
        };
        NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS gpu_info = {
            .gpuId = probed_ids.gpuIds[i]
        };
        NV0080_ALLOC_PARAMETERS dev_alloc = {};
        NV2080_ALLOC_PARAMETERS sub_dev_alloc = {};
        NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS bus_info = {};

        struct NvGpu *ngpu = calloc(1, sizeof(struct NvGpu));
        struct Gpu *gpu = &(ret->gpus[i]);

        uint32_t device = dat->device_ids(
            root->object, probed_ids.gpuIds[i], 1
        );
        uint32_t subdevice = dat->device_ids(
            root->object, probed_ids.gpuIds[i], 2
        );

        if (ngpu == NULL) {
            printf("Could not allocate NVIDIA vendor information for the GPU.\n");
            goto failed_gpus;
        }

        gpu->vendor_info = ngpu;

        attach_ids.gpuIds[0] = probed_ids.gpuIds[i];
        attach_ids.gpuIds[1] = NV0000_CTRL_GPU_INVALID_ID;

        RM_CTRL(ctl_fd, root, NV0000_CTRL_CMD_GPU_GET_PCI_INFO, pci_info);
        RM_CTRL(ctl_fd, root, NV0000_CTRL_CMD_GPU_ATTACH_IDS, attach_ids);

        ngpu->fd = nv_open_dev(i);

        RM_CTRL(ctl_fd, root, NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2, gpu_info);

        dev_alloc.deviceId = gpu_info.deviceInstance;

        if (dat->share_type == RS_SHARE_TYPE_CLIENT)
            dev_alloc.hClientShare = root->object;

        sub_dev_alloc.subDeviceId = 0;

        ngpu->dev = rm_alloc_res(ctl_fd, root, device, NV01_DEVICE_0, &dev_alloc);

        if (ngpu->dev == NULL) {
            printf("Could not allocate device: 0x%.8X\n", device);
            goto failed_gpus;
        }

        ngpu->dev->class_info = gpu;

        ngpu->sub_dev = rm_alloc_res(
            ctl_fd, ngpu->dev, subdevice, NV20_SUBDEVICE_0, &sub_dev_alloc
        );

        if (ngpu->sub_dev == NULL) {
            printf("Could not allocate subdevice: 0x%.8X\n", device);
            goto failed_gpus;
        }

        RM_CTRL(ctl_fd, ngpu->sub_dev, NV2080_CTRL_CMD_BUS_GET_PCI_INFO, bus_info);

        gpu->identifier = probed_ids.gpuIds[i];
        gpu->number = i;
        gpu->domain = pci_info.domain;
        gpu->bus = pci_info.bus;
        gpu->slot = pci_info.slot;
        gpu->vendor_id = 0x10DE;
        gpu->device_id = bus_info.pciDeviceId >> 16;
        gpu->sub_vendor_id = 0x10DE;
        gpu->sub_device_id = bus_info.pciSubSystemId >> 16;

        if (get_used_fb(in, gpu)) {
            printf("Could not get framebuffer for gpu%d.\n", gpu->number);
            goto failed_gpus;
        }

        printf(
            "Attached GPU %.8X (%.4X, %.4X, %.4X, %.4X)\n"
            "\tfree_fb: %ld MBs\n"
            "\ttotal_fb: %ld MBs\n",
            gpu->identifier, gpu->vendor_id, gpu->device_id,
            gpu->sub_vendor_id, gpu->sub_device_id,
            gpu->free_fb / 1024 / 1024, gpu->total_fb / 1024 / 1024
        );
    }

    ret->num_gpus = i;
    dat->gpus = ret;

    printf("Attached all GPUs correctly\n");

    return ret;

failed_gpus:
    rm_free_tree(ctl_fd, root);

failed:
    free(ret);
    ret = NULL;

    return ret;
}

/*! \brief Stops the NVIDIA code from running.
 */
static uint8_t stop(void *in)
{
    struct NvInitData *dat = (struct NvInitData*) in;

    rm_free_tree(dat->ctl_fd, dat->root);

    free(dat->gpus);

    return 0;
}

const struct InitializationAPI nvidia_initialization_api = {
.init = init,
.get_gpus = get_gpus,
.get_used_fb = get_used_fb,
.stop = stop
};
