/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <gpu/nvidia/device.h>
#include <gpu/nvidia/resman.h>

#include <utils/types.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <class/cl0080.h>
#include <class/cl2080.h>
#include <class/cla081.h>

#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl2080/ctrl2080bus.h>
#include <ctrl/ctrl2080/ctrl2080fb.h>
#include <ctrl/ctrla081.h>

static inline uint32_t compose_manager_id(uint32_t client, uint32_t gpu_id, uint32_t handle)
{
    return ((client + gpu_id) << 8) | (handle & ((1 << 8) - 1));
}

struct NvResource *init_root(const uint8_t ignore_version)
{
    int fd = nv_open_dev(255);

    if (!rm_version_check(fd, ignore_version, RM_VERSION)) {
        printf("Incorrect version of RM APIs.\n");
        return NULL;
    }

    return rm_alloc_res(fd, NULL, 0, 0, NULL);
}

int get_gpu(struct NvResource *root, uint32_t device, uint32_t subdevice, struct Gpu* gpu)
{
    int fd = -1;
    NV0000_CTRL_GPU_ATTACH_IDS_PARAMS attach_ids = {};
    NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS pci_info = {
        .gpuId = gpu->identifier
    };
    NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS gpu_info = {
        .gpuId = gpu->identifier
    };
    NV0080_ALLOC_PARAMETERS dev_alloc = {};
    NV2080_ALLOC_PARAMETERS sub_dev_alloc = {};
    NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS bus_info = {};
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS fb_info = {};
    struct NvResource *dev = NULL;
    struct NvResource *sdev = NULL;

    if (root == NULL || gpu == NULL)
        return fd;

    attach_ids.gpuIds[0] = gpu->identifier;
    attach_ids.gpuIds[1] = NV0000_CTRL_GPU_INVALID_ID;

    RM_CTRL(root->fd, root, NV0000_CTRL_CMD_GPU_GET_PCI_INFO, pci_info);
    RM_CTRL(root->fd, root, NV0000_CTRL_CMD_GPU_ATTACH_IDS, attach_ids);

    fd = nv_open_dev(gpu->number);

    RM_CTRL(root->fd, root, NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2, gpu_info);

    dev_alloc.deviceId = gpu_info.deviceInstance;
    sub_dev_alloc.subDeviceId = 0;

    dev = rm_alloc_res(root->fd, root, device, NV01_DEVICE_0, &dev_alloc);

    dev->class_info = gpu;

    sdev = rm_alloc_res(root->fd, dev, subdevice, NV20_SUBDEVICE_0, &sub_dev_alloc);

    fb_info.fbInfoListSize = 2;
    fb_info.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE;
    fb_info.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE;

    RM_CTRL(root->fd, sdev, NV2080_CTRL_CMD_BUS_GET_PCI_INFO, bus_info);
    RM_CTRL(root->fd, sdev, NV2080_CTRL_CMD_FB_GET_INFO_V2, fb_info);

    gpu->domain = pci_info.domain;
    gpu->bus = pci_info.bus;
    gpu->slot = pci_info.slot;
    gpu->vendor_id = 0x10DE;
    gpu->device_id = bus_info.pciDeviceId >> 16;
    gpu->sub_vendor_id = 0x10DE;
    gpu->sub_device_id = bus_info.pciSubSystemId >> 16;

    gpu->free_fb = ((uint64_t) fb_info.fbInfoList[0].data) << 10;
    gpu->total_fb = ((uint64_t) fb_info.fbInfoList[1].data) << 10;

    return fd;
}
