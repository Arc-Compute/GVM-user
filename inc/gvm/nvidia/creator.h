/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_NVIDIA_CREATOR_H
#define GVM_NVIDIA_CREATOR_H

#include <gpu/nvidia/init.h>

#include <ctrl/ctrla081.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Index for the vGPU configurator.
#define VGPU_CONFIG_INDEX 0

//! License for the vGPUs created by us for the NVIDIA subsystem.
#define VGPU_LICENSE_COMPUTE "NVIDIA-vComputeServer,9.0;Quadro-Virtual-DWS,5.0"

/*! \brief Internal creator structure for NVIDIA.
 */
struct NvCreator {
    struct NvInitData *dat;               //!< Data for the NVIDIA driver.
    void (*create_signature)(
        NVA081_CTRL_VGPU_INFO*
    );                                    //!< Function to generate a signature.
    uint8_t ignore_version_check;         //!< If we care about version mismatch.
};

#ifdef __cplusplus
};
#endif

#endif
