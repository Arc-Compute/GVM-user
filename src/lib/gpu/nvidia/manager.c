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
#include <gpu/nvidia/manager.h>
#include <gpu/nvidia/resman.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <class/cl0080.h>
#include <class/cl2080.h>
#include <class/cla081.h>

#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl2080/ctrl2080bus.h>
#include <ctrl/ctrla081.h>

//! Hardcoded signature.
static const char SIGN[128] = {
#if 0
0x47, 0xab, 0x8d, 0x39, 0xb3, 0xaf, 0xd0, 0x2c, 0x79, 0x6f, 0xd0, 0xc7, 0x7a, 0x78, 0x84, 0x68,
0xf0, 0x9b, 0x69, 0xe8, 0xb6, 0xc2, 0xc5, 0x05, 0x59, 0x97, 0xf2, 0x0f, 0x77, 0x3a, 0x94, 0x91,
0x92, 0x56, 0x2d, 0xf0, 0x4f, 0xae, 0xa6, 0x4d, 0xcc, 0x51, 0x32, 0x17, 0xe5, 0xda, 0xf0, 0x94,
0x42, 0x93, 0x51, 0x05, 0x49, 0xe9, 0x61, 0xfd, 0x22, 0x24, 0x6c, 0x8f, 0x88, 0xe3, 0x16, 0x63,
0x91, 0x04, 0x20, 0x6a, 0xea, 0x27, 0xc4, 0xe7, 0x11, 0xfc, 0x88, 0x24, 0xb9, 0xaa, 0x1b, 0x85,
0xc5, 0x1a, 0x48, 0x7f, 0x99, 0xf4, 0x8e, 0xda, 0x55, 0x2b, 0x4a, 0xfe, 0x48, 0x79, 0x75, 0x78,
0x48, 0x16, 0x02, 0x0f, 0x22, 0xc2, 0x9d, 0x20, 0xfa, 0xbb, 0x21, 0x39, 0x56, 0x78, 0xd8, 0x80,
0x96, 0x5c, 0x5c, 0xe4, 0x7c, 0xad, 0x87, 0x24, 0x20, 0x70, 0xad, 0x63, 0x84, 0x96, 0x92, 0x3f
#else
0x12, 0x57, 0xc7, 0xf4, 0x5a, 0x14, 0x60, 0xba, 0x66, 0xaf, 0x59, 0xe5, 0x20, 0x5b, 0x7c, 0x4a,
0x0a, 0x16, 0x8f, 0xac, 0xd8, 0x52, 0xa2, 0x45, 0xb5, 0xf1, 0x64, 0xc7, 0xfd, 0x40, 0x1d, 0x2c,
0xa9, 0x58, 0x7d, 0x57, 0x39, 0xa2, 0x8f, 0x34, 0x7a, 0x3e, 0x5b, 0xfb, 0x47, 0x08, 0x30, 0x1b,
0xbd, 0x25, 0x86, 0xeb, 0x27, 0x7d, 0x56, 0x11, 0x7c, 0x1f, 0xba, 0xc9, 0xa2, 0x78, 0xbc, 0xd0,
0x1f, 0xe4, 0x5e, 0xd9, 0x3e, 0x29, 0xba, 0x97, 0xd6, 0xf8, 0xeb, 0xff, 0x9a, 0x95, 0xa4, 0x53,
0x95, 0xc1, 0xbe, 0x47, 0x8f, 0xad, 0xff, 0x84, 0xd2, 0xa8, 0xe2, 0xfc, 0x4f, 0x34, 0x29, 0x84,
0x4b, 0x21, 0x0b, 0xe2, 0x72, 0x17, 0xe6, 0x91, 0xf2, 0xbb, 0x0f, 0xd1, 0x51, 0x52, 0x38, 0x67,
0x73, 0x5a, 0x75, 0x95, 0x4c, 0x85, 0x93, 0x06, 0x49, 0x26, 0x9a, 0xea, 0x59, 0x61, 0xb8, 0xcb
#endif
};

/*! \brief Inline function to create device ids. */
static inline uint32_t compose_manager_id(uint32_t client, uint32_t gpu_id, uint32_t handle)
{
    return ((client + gpu_id) << 8) | (handle & ((1 << 8) - 1));
}

/*! \todo Use proper logging.
 */
struct NvMdev create_nv_mgr()
{
    struct NvMdev ret = {};
    NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS probed_ids = {};

    ret.fd = nv_open_dev(255);
    ret.res = rm_alloc_res(ret.fd, NULL, 0, 0, NULL);

    if (ret.res == NULL)
        goto failure;

    if (RM_CTRL(ret.fd, ret.res, NV0000_CTRL_CMD_GPU_GET_PROBED_IDS, probed_ids) == NULL)
        goto free_failure;

    for (int i = 0; i < NV0000_CTRL_GPU_MAX_PROBED_GPUS && probed_ids.gpuIds[i] != NV0000_CTRL_GPU_INVALID_ID; ++i) {
        struct NvMdevGpu *mgpu = calloc(1, sizeof(struct NvMdevGpu));

        mgpu->ctl_fd = ret.fd;
        mgpu->gpu = calloc(1, sizeof(struct Gpu));
        mgpu->root = ret.res->object;
        mgpu->device = compose_manager_id(ret.res->object, probed_ids.gpuIds[i], 1);
        mgpu->sub_device = compose_manager_id(ret.res->object, probed_ids.gpuIds[i], 2);
        mgpu->mdev_config = compose_manager_id(ret.res->object, probed_ids.gpuIds[i], 3);

        NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS pci_info = {
           .gpuId = probed_ids.gpuIds[i]
        };
        NV0000_CTRL_GPU_ATTACH_IDS_PARAMS attach_ids = {};

        attach_ids.gpuIds[0] = probed_ids.gpuIds[i];
        attach_ids.gpuIds[1] = NV0000_CTRL_GPU_INVALID_ID;

        RM_CTRL(ret.fd, ret.res, NV0000_CTRL_CMD_GPU_GET_PCI_INFO, pci_info);
        RM_CTRL(ret.fd, ret.res, NV0000_CTRL_CMD_GPU_ATTACH_IDS, attach_ids);

        mgpu->dev_fd = nv_open_dev(i);

        NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS gpu_info = {};
        NV0080_ALLOC_PARAMETERS dev_alloc = {};
        NV2080_ALLOC_PARAMETERS sub_dev_alloc = {};

        gpu_info.gpuId = probed_ids.gpuIds[i];

        RM_CTRL(ret.fd, ret.res, NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2, gpu_info);

        dev_alloc.deviceId = gpu_info.deviceInstance;
        dev_alloc.hClientShare = ret.res->object;

        sub_dev_alloc.subDeviceId = 0;

        mgpu->dev = rm_alloc_res(mgpu->ctl_fd, ret.res, mgpu->device, NV01_DEVICE_0, &dev_alloc);
        mgpu->sdev = rm_alloc_res(mgpu->ctl_fd, mgpu->dev, mgpu->sub_device, NV20_SUBDEVICE_0, &sub_dev_alloc);

        NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS bus_info = {};

        RM_CTRL(ret.fd, mgpu->sdev, NV2080_CTRL_CMD_BUS_GET_PCI_INFO, bus_info);

        mgpu->mdev = rm_alloc_res(mgpu->ctl_fd, mgpu->sdev, mgpu->mdev_config, NVA081_VGPU_CONFIG, NULL);

        mgpu->dev->class_info = mgpu->gpu;

        mgpu->gpu->identifier = probed_ids.gpuIds[i];
        mgpu->gpu->domain = pci_info.domain;
        mgpu->gpu->bus = pci_info.bus;
        mgpu->gpu->slot = pci_info.slot;
        mgpu->gpu->vendor_id = 0x10DE;
        mgpu->gpu->device_id = bus_info.pciDeviceId >> 16;
        mgpu->gpu->sub_vendor_id = 0x10DE;
        mgpu->gpu->sub_device_id = bus_info.pciSubSystemId >> 16;

        ret.gpus[i] = mgpu;

        printf(
            "Created gpu: 0x%.8X (0x%.4X, 0x%.4X, 0x%.4X, 0x%.4X)\n",
            ret.gpus[i]->gpu->identifier,
            ret.gpus[i]->gpu->vendor_id,
            ret.gpus[i]->gpu->device_id,
            ret.gpus[i]->gpu->sub_vendor_id,
            ret.gpus[i]->gpu->sub_device_id
        );
    }

    return ret;

free_failure:
    for (int i = 0; i < NV0000_CTRL_GPU_MAX_PROBED_GPUS && ret.gpus[i] != NULL; ++i) {
        printf(
            "Destroyed gpu: 0x%.8X (0x%.4X, 0x%.4X, 0x%.4X, 0x%.4X)\n",
            ret.gpus[i]->gpu->identifier,
            ret.gpus[i]->gpu->vendor_id,
            ret.gpus[i]->gpu->device_id,
            ret.gpus[i]->gpu->sub_vendor_id,
            ret.gpus[i]->gpu->sub_device_id
        );
        free(ret.gpus[i]);
        ret.gpus[i] = NULL;
    }

    rm_free_tree(ret.fd, ret.res);
    ret.res = NULL;

failure:
    close(ret.fd);
    ret.fd = -1;
    return ret;
}

/*! \todo Use proper logging.
 */
void free_nv_mgr(struct NvMdev *mgr)
{
    if (mgr->fd == -1)
        return;

    for (int i = 0; i < NV0000_CTRL_GPU_MAX_PROBED_GPUS && mgr->gpus[i] != NULL; ++i) {
        printf(
            "Destroyed gpu: 0x%.8X (0x%.4X, 0x%.4X, 0x%.4X, 0x%.4X)\n",
            mgr->gpus[i]->gpu->identifier,
            mgr->gpus[i]->gpu->vendor_id,
            mgr->gpus[i]->gpu->device_id,
            mgr->gpus[i]->gpu->sub_vendor_id,
            mgr->gpus[i]->gpu->sub_device_id
        );
        free(mgr->gpus[i]);
        mgr->gpus[i] = NULL;
    }

    rm_free_tree(mgr->fd, mgr->res);
    mgr->res = NULL;

    close(mgr->fd);
    mgr->fd = -1;
}

void create_nv_mgr_mdevs(
    struct NvMdev *mgr,
    struct Gpu* limited,
    size_t gpu_size,
    struct MDevRequest* requested,
    size_t mdev_size
)
{
    for (int i = 0; i < NV0000_CTRL_GPU_MAX_PROBED_GPUS && mgr->gpus[i] != NULL && limited != NULL; ++i) {
        struct NvMdevGpu *gpu = mgr->gpus[i];
        struct Gpu *ggpu = gpu->gpu;
        uint8_t valid = 0;

        for (size_t j = 0; j < gpu_size && !valid; ++j) {
            struct Gpu req_gpu = limited[j];

            valid =
                (req_gpu.domain == 0xFFFFFFFF ||
                 req_gpu.domain == ggpu->domain) &&
                (req_gpu.bus == 0xFFFFFFFF ||
                 req_gpu.bus == ggpu->bus) &&
                (req_gpu.slot == 0xFFFFFFFF ||
                 req_gpu.slot == ggpu->slot) &&
                (req_gpu.function == 0xFFFFFFFF ||
                 req_gpu.function == ggpu->function) &&
                (req_gpu.vendor_id == 0xFFFFFFFF ||
                 req_gpu.vendor_id == ggpu->vendor_id) &&
                (req_gpu.device_id == 0xFFFFFFFF ||
                 req_gpu.device_id == ggpu->device_id) &&
                (req_gpu.sub_vendor_id == 0xFFFFFFFF ||
                 req_gpu.sub_vendor_id == ggpu->sub_vendor_id) &&
                (req_gpu.sub_device_id == 0xFFFFFFFF ||
                 req_gpu.sub_device_id == ggpu->sub_device_id) &&
                (req_gpu.identifier == 0xFFFFFFFF ||
                 req_gpu.identifier == ggpu->identifier);
        }

        if (!valid)
            continue;

        for (size_t j = 0; j < mdev_size; ++j) {
            struct MDevRequest request = requested[j];
            NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS mdev = {};
            NVA081_CTRL_VGPU_INFO *info = &mdev.vgpuInfo;

            mdev.discardVgpuTypes = j == 0;
            info->vgpuType = request.num;
            strcpy((char*) info->vgpuName, request.name);
            strcpy((char*) info->vgpuClass, request.gpu_class);
            memcpy(info->vgpuSignature, SIGN, 128);
            strcpy((char*) info->license, "NVIDIA-vComputeServer,9.0;Quadro-Virtual-DWS,5.0");
            info->maxInstance = request.max_inst;

            if (request.disp != NULL) {
                info->numHeads = request.disp->num_heads;
                info->maxResolutionX = request.disp->max_res_x;
                info->maxResolutionY = request.disp->max_res_y;
                info->maxPixels = request.disp->max_res_x * request.disp->max_res_y;
                info->frlConfig = request.disp->frl_config;
                info->frlEnable = request.disp->frl_enable;
            }

            info->cudaEnabled = 0;
            info->eccSupported = request.ecc_support;
            info->gpuInstanceSize = 0;
            info->multiVgpuSupported = request.multi_mdev;
            info->encoderCapacity = request.enc_cap;
            info->vdevId =
                request.v_dev_id == 0xFFFFFFFFFFFFFFFF ?
                (ggpu->device_id << 16 || ggpu->sub_device_id) :
                request.v_dev_id;
            info->pdevId =
                request.p_dev_id == 0xFFFFFFFFFFFFFFFF ?
                ggpu->device_id : request.p_dev_id;
            info->fbLength = ((uint64_t) request.fb_len) * 1024 * 1024;
            info->mappableVideoSize = ((uint64_t) request.map_vid_size) * 1024 * 1024;
            info->fbReservation = ((uint64_t) request.fb_res) * 1024 * 1024;
            info->bar1Length = request.bar1_len;

            /* mdev.vgpuConfigState = */
            /*     j == mdev_size ? */
            /*     NVA081_CTRL_VGPU_CONFIG_STATE_READY : NVA081_CTRL_VGPU_CONFIG_STATE_IN_PROGRESS; */

            RM_CTRL(gpu->ctl_fd, gpu->mdev, NVA081_CTRL_CMD_VGPU_CONFIG_SET_INFO, mdev);
        }
    }
}

void register_nv_mgr_mdevs(struct NvMdev* mgr)
{
    for (int i = 0; i < NV0000_CTRL_GPU_MAX_PROBED_GPUS && mgr->gpus[i] != NULL; ++i) {
        struct NvMdevGpu* gpu = mgr->gpus[i];

        rm_ctrl_res(
            gpu->ctl_fd,
            gpu->root,
            gpu->mdev_config,
            NVA081_CTRL_CMD_VGPU_CONFIG_MDEV_REGISTER,
            NULL,
            0
        );
    }
}
