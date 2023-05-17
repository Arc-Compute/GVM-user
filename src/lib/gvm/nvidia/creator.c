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
#include <gvm/nvidia/debug/envy.h>

#include <utils/gpus.h>

#include <class/cla081.h>

#include <ctrl/ctrla081.h>

#include <rs_access.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//! Initialization API.
#define api INIT_APIS[Nvidia]

/*! \brief Creator device id codebase.*/
static uint32_t creator_ids(
    uint32_t client, uint32_t gpu_id, uint32_t handle
)
{
    return ((client + gpu_id) << 8) | (handle & ((1 << 8) - 1));
}

/*! \brief Initialization Code for NVIDIA.
 *
 * This code initializes the NVIDIA GVM system for creation.
 */
static uint8_t init(void *in)
{
    struct NvCreator *crt = (struct NvCreator*) in;
    struct NvInitData *dat = NULL;

    if (crt == NULL)
        return 1;

    crt->dat = (struct NvInitData*) calloc(1, sizeof(struct NvInitData));

    dat = crt->dat;

    if (dat == NULL)
        return 1;

    dat->device_ids = creator_ids;
    dat->share_type = RS_SHARE_TYPE_NONE;
    dat->ignore_version_check = crt->ignore_version_check;

    if (api->init((void*) crt->dat))
        goto failed_init;

    struct AttachedGpus *gpus = api->get_gpus(crt->dat);

    if (gpus == NULL)
        goto failed;

    for (uint32_t i = 0; i < gpus->num_gpus; ++i) {
        struct NvGpu *ngpu = (struct NvGpu*) gpus->gpus[i].vendor_info;
        uint32_t vgpu_config_id = dat->device_ids(
            dat->root->object, gpus->gpus[i].identifier, 3
        );

        if (ngpu == NULL) continue;

        ngpu->engines[VGPU_CONFIG_INDEX] = rm_alloc_res(
            dat->ctl_fd, ngpu->sub_dev, vgpu_config_id, NVA081_VGPU_CONFIG, NULL
        );

        if (ngpu->engines[VGPU_CONFIG_INDEX] == NULL) {
            printf("Failed to create vGPU configuration engine.\n");
            goto failed;
        }
    }

    printf("Created the GVM-Creator on a NVIDIA system.\n");

    return 0;

failed:
    api->stop(dat);

failed_init:
    free(dat);

    crt->dat = NULL;

    return 1;
}

/*! \brief Stops the NVIDIA Creator API.
 */
static uint8_t stop(void *in)
{
    struct NvCreator *crt = (struct NvCreator*) in;

    if (crt == NULL)
        return 1;

    api->stop(crt->dat);

    free(crt->dat);

    crt->dat = NULL;

    return 0;
}

/*! \brief Function to create the mediated devices in the kernel module.
 *
 * This function serves as a mechanism to create different forms of mediated
 * devices.
 *
 * NOTE: Current iterations of the NVIDIA Kernel module require a vgpu
 *       signature bypass to submit these requests, we hope that future
 *       iterations will not.
 *
 * TODO: USE PROPER LOGGING.
 */
static uint8_t create_mdevs(
    void *in,
    struct MDevRequest *requested, size_t mdev_length,
    struct Gpu *restricted, size_t restrict_length
)
{
    struct NvCreator *crt = (struct NvCreator*) in;
    struct NvInitData *dat = NULL;
    struct AttachedGpus *gpus = NULL;

    printf("function: %s+0x%X\n", probe.function, probe.offset);

    if (crt == NULL || crt->dat == NULL || crt->dat->gpus == NULL ||
        requested == NULL || restricted == NULL)
        return 1;

    dat = crt->dat;
    gpus = crt->dat->gpus;

    for (uint32_t i = 0; i < gpus->num_gpus; ++i) {
        struct Gpu *gpu = (struct Gpu*) &(gpus->gpus[i]);
        struct NvGpu *ngpu = (struct NvGpu*) gpu->vendor_info;
        uint8_t valid = 0;

        if (ngpu == NULL) continue;

        printf("Checking if GPU %d is valid...\n", gpu->number);

        for (size_t j = 0; j < restrict_length && !valid; ++j)
            valid = valid_gpu(gpu, restricted + j);

        if (!valid)
            continue;

        for (size_t j = 0; j < mdev_length; ++j) {
            struct MDevRequest request = requested[j];

            NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS vgpu = {};
            NVA081_CTRL_VGPU_INFO *info = &vgpu.vgpuInfo;

            printf("Filling up vGPU %d structure...\n", request.num);

            vgpu.discardVgpuTypes = j == 0;

            info->vgpuType = request.num;
            strcpy((char*) info->vgpuName, request.name);
            strcpy((char*) info->vgpuClass, request.gpu_class);
            strcpy((char*) info->license, VGPU_LICENSE_COMPUTE);
            info->maxInstance = request.max_inst;

            if (request.disp != NULL) {
                info->numHeads = request.disp->num_heads;
                info->maxResolutionX = request.disp->max_res_x;
                info->maxResolutionY = request.disp->max_res_y;
                info->maxPixels = request.disp->max_res_x * request.disp->max_res_y;
                info->frlConfig = request.disp->frl_config;
                info->frlEnable = request.disp->frl_enable;
            }

            info->cudaEnabled = 1;
            info->eccSupported = request.ecc_support;
            info->gpuInstanceSize = 0;
            info->multiVgpuSupported = request.multi_mdev;
            info->encoderCapacity = request.enc_cap;
            info->vdevId =
                request.v_dev_id == 0xFFFFFFFFFFFFFFFF ?
                (gpu->device_id << 16 || gpu->sub_device_id) :
                request.v_dev_id;
            info->pdevId =
                request.p_dev_id == 0xFFFFFFFFFFFFFFFF ?
                gpu->device_id : request.p_dev_id;
            info->fbLength = ((uint64_t) request.fb_len) * 1024 * 1024;
            info->mappableVideoSize =
                ((uint64_t) request.map_vid_size) * 1024 * 1024;
            info->fbReservation = ((uint64_t) request.fb_res) * 1024 * 1024;
            info->profileSize = info->fbLength + info->fbReservation;
            info->bar1Length = request.bar1_len;

            if (crt->create_signature != NULL)
                crt->create_signature(info);

            vgpu.vgpuConfigState =
                j == mdev_length ?
                NVA081_CTRL_VGPU_CONFIG_STATE_READY :
                NVA081_CTRL_VGPU_CONFIG_STATE_IN_PROGRESS;

            printf(
                "Sending vgpu configuration %d for GPU %d...\n",
                info->vgpuType, gpu->number
            );

            RM_CTRL(
                dat->ctl_fd,
                ngpu->engines[VGPU_CONFIG_INDEX],
                NVA081_CTRL_CMD_VGPU_CONFIG_SET_INFO,
                vgpu
            );

            printf(
                "Sent vgpu configuration %d for GPU %d\n",
                info->vgpuType, gpu->number
            );
        }
    }

    return 0;
}

/*! \brief Function to register the mdevs.
 *
 * TODO: USE PROPER LOGGING.
 */
static uint8_t register_mdevs(void *in)
{
    struct NvCreator *crt = (struct NvCreator*) in;
    struct NvInitData *dat = NULL;
    struct AttachedGpus *gpus = NULL;

    if (crt == NULL || crt->dat == NULL || crt->dat->gpus == NULL)
        return 1;

    dat = crt->dat;
    gpus = dat->gpus;

    printf("Registering NVIDIA vGPUs...\n");

    for (uint32_t i = 0; i < gpus->num_gpus; ++i) {
        struct Gpu *gpu = (struct Gpu*) &(gpus->gpus[i]);
        struct NvGpu *ngpu = (struct NvGpu*) gpu->vendor_info;

        if (ngpu == NULL) continue;

        printf("Registering mdevs on gpu %d\n", gpu->number);

        rm_ctrl_res(
            dat->ctl_fd,
            dat->root->object,
            ngpu->engines[VGPU_CONFIG_INDEX]->object,
            NVA081_CTRL_CMD_VGPU_CONFIG_MDEV_REGISTER,
            NULL,
            0
        );
    }

    return 0;
}

const struct CreatorAPI nvidia_creator_api = {
.init = init,
.create_mdevs = create_mdevs,
.register_mdevs = register_mdevs,
.stop = stop
};
