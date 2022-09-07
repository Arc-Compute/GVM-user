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
#include <gpu/nvidia/resman/api.h>

#include <gpu/nvidia/resman/classes.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//! Hardcoded signature.
static const char SIGN[128] = {
0x47, 0xab, 0x8d, 0x39, 0xb3, 0xaf, 0xd0, 0x2c, 0x79, 0x6f, 0xd0, 0xc7, 0x7a, 0x78, 0x84, 0x68,
0xf0, 0x9b, 0x69, 0xe8, 0xb6, 0xc2, 0xc5, 0x05, 0x59, 0x97, 0xf2, 0x0f, 0x77, 0x3a, 0x94, 0x91,
0x92, 0x56, 0x2d, 0xf0, 0x4f, 0xae, 0xa6, 0x4d, 0xcc, 0x51, 0x32, 0x17, 0xe5, 0xda, 0xf0, 0x94,
0x42, 0x93, 0x51, 0x05, 0x49, 0xe9, 0x61, 0xfd, 0x22, 0x24, 0x6c, 0x8f, 0x88, 0xe3, 0x16, 0x63,
0x91, 0x04, 0x20, 0x6a, 0xea, 0x27, 0xc4, 0xe7, 0x11, 0xfc, 0x88, 0x24, 0xb9, 0xaa, 0x1b, 0x85,
0xc5, 0x1a, 0x48, 0x7f, 0x99, 0xf4, 0x8e, 0xda, 0x55, 0x2b, 0x4a, 0xfe, 0x48, 0x79, 0x75, 0x78,
0x48, 0x16, 0x02, 0x0f, 0x22, 0xc2, 0x9d, 0x20, 0xfa, 0xbb, 0x21, 0x39, 0x56, 0x78, 0xd8, 0x80,
0x96, 0x5c, 0x5c, 0xe4, 0x7c, 0xad, 0x87, 0x24, 0x20, 0x70, 0xad, 0x63, 0x84, 0x96, 0x92, 0x3f
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
    struct Nv0000CtrlGpuGetProbedIdsParams probed_ids = {};

    ret.fd = nv_open_dev(255);

    ret.res = rm_alloc_res(ret.fd, NULL, 0, 0, NULL);

    if (ret.res == NULL)
        goto failure;

    if (RM_CTRL(ret.fd, ret.res, NV0000_GET_PROBED_IDS, probed_ids) == NULL)
        goto free_failure;

    for (int i = 0; i < 32 && probed_ids.gpu_ids[i] != 0xFFFFFFFF; ++i) {
        struct NvMdevGpu *mgpu = calloc(1, sizeof(struct NvMdevGpu));

        mgpu->ctl_fd = ret.fd;
        mgpu->gpu = calloc(1, sizeof(struct Gpu));
        mgpu->root = ret.res->object;
        mgpu->device = compose_manager_id(ret.res->object, probed_ids.gpu_ids[i], 1);
        mgpu->sub_device = compose_manager_id(ret.res->object, probed_ids.gpu_ids[i], 2);
        mgpu->mdev_config = compose_manager_id(ret.res->object, probed_ids.gpu_ids[i], 3);

        struct Nv0000CtrlGpuGetPciInfoParams pci_info = {
           .gpu_id = probed_ids.gpu_ids[i]
        };
        struct Nv0000CtrlGpuAttachIdsParams attach_ids = {};

        attach_ids.gpu_ids[0] = probed_ids.gpu_ids[i];
        attach_ids.gpu_ids[1] = 0xFFFFFFFF;

        RM_CTRL(ret.fd, ret.res, NV0000_GET_PCI_INFO, pci_info);
        RM_CTRL(ret.fd, ret.res, NV0000_ATTACH_IDS, attach_ids);

        mgpu->dev_fd = nv_open_dev(i);

        struct Nv0000CtrlGpuGetIdInfoParams gpu_info = {};
        struct Nv0080AllocParams dev_alloc = {};
        uint32_t sub_dev_alloc = 0;

        gpu_info.gpu_id = probed_ids.gpu_ids[i];

        RM_CTRL(ret.fd, ret.res, NV0000_GET_GPU_INFO, gpu_info);

        dev_alloc.deviceId = gpu_info.dev_inst;
        dev_alloc.hClientShare = ret.res->object;

        mgpu->dev = rm_alloc_res(mgpu->ctl_fd, ret.res, mgpu->device, NV0080_CLASS, &dev_alloc);
        mgpu->sdev = rm_alloc_res(mgpu->ctl_fd, mgpu->dev, mgpu->sub_device, NV2080_CLASS, &sub_dev_alloc);

        struct BusGetPciInfo bus_info = {};

        RM_CTRL(ret.fd, mgpu->sdev, NV2080_GET_BUS_PCI_INFO, bus_info);

        mgpu->mdev = rm_alloc_res(mgpu->ctl_fd, mgpu->sdev, mgpu->mdev_config, NVA081_CLASS, NULL);

        mgpu->dev->class_info = mgpu->gpu;

        mgpu->gpu->identifier = probed_ids.gpu_ids[i];
        mgpu->gpu->domain = pci_info.domain;
        mgpu->gpu->bus = pci_info.bus;
        mgpu->gpu->slot = pci_info.slot;
        mgpu->gpu->vendor_id = 0x10DE;
        mgpu->gpu->device_id = bus_info.dev_id >> 16;
        mgpu->gpu->sub_vendor_id = 0x10DE;
        mgpu->gpu->sub_device_id = bus_info.sub_dev_id >> 16;

        printf(
            "Created gpu: 0x%.8X (0x%.4X, 0x%.4X, 0x%.4X, 0x%.4X)\n",
            probed_ids.gpu_ids[i],
            mgpu->gpu->vendor_id,
            mgpu->gpu->device_id,
            mgpu->gpu->sub_vendor_id,
            mgpu->gpu->sub_device_id
        );

        ret.gpus[i] = mgpu;
    }

    return ret;

free_failure:
    for (int i = 0; i < 32 && ret.gpus[i] != NULL; ++i) {
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

    for (int i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
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
    for (int i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct NvMdevGpu* gpu = mgr->gpus[i];
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

        if (!valid && limited != NULL && gpu_size > 0)
            continue;

        for (size_t j = 0; j < mdev_size; ++j) {
            struct MDevRequest request = requested[j];
            struct RmMdevConfig mdev = {};

            mdev.discard = j == 0;
            mdev.mdev_type = request.num;
            strcpy(mdev.name, request.name);
            strcpy(mdev.class, request.gpu_class);
            memcpy(mdev.sign, SIGN, 128);
            strcpy(mdev.pact, "NVIDIA-vComputeServer,9.0;Quadro-Virtual-DWS,5.0");
            mdev.max_instances = request.max_inst;

            if (request.disp != NULL) {
                mdev.num_heads = request.disp->num_heads;
                mdev.max_res_x = request.disp->max_res_x;
                mdev.max_res_y = request.disp->max_res_y;
                mdev.max_pixel = request.disp->max_res_x * request.disp->max_res_y;
                mdev.frl_config = request.disp->frl_config;
                mdev.frl_enable = request.disp->frl_enable;
            }

            mdev.cuda = 1;
            mdev.ecc_support = request.ecc_support;
            mdev.gpu_instance_size = 0;
            mdev.multi_mdev = request.multi_mdev;
            mdev.enc_cap = request.enc_cap;
            mdev.v_dev_id =
                request.v_dev_id == 0xFFFFFFFFFFFFFFFF ?
                (ggpu->device_id << 16 || ggpu->sub_device_id) :
                request.v_dev_id;
            mdev.p_dev_id =
                request.p_dev_id == 0xFFFFFFFFFFFFFFFF ?
                ggpu->device_id : request.p_dev_id;
            mdev.fb_len = request.fb_len * 1024 * 1024;
            mdev.map_video = request.map_vid_size * 1024 * 1024;
            mdev.fb_res = request.fb_res * 1024 * 1024;
            mdev.bar1_len = request.bar1_len;

            RM_CTRL(gpu->ctl_fd, gpu->mdev, NVA081_ADD_MDEV, mdev);
        }
    }
}

void register_nv_mgr_mdevs(struct NvMdev *mgr)
{
    for (int i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct NvMdevGpu* gpu = mgr->gpus[i];

        rm_ctrl_res(
            gpu->ctl_fd,
            gpu->root,
            gpu->mdev_config,
            NVA081_REG_MDEV,
            NULL,
            0
        );
    }
}
