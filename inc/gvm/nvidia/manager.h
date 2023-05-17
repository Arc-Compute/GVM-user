/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_NVIDIA_MANAGER_H
#define GVM_NVIDIA_MANAGER_H

#include <gpu/nvidia/init.h>
#include <gvm/nvidia/creator.h>

#include <ctrl/ctrla081.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Internal Manager structure for NVIDIA.
 */
struct NvManager {
    struct NvInitData *mgr;               //!< Data for the NVIDIA driver.
    struct NvCreator *crt;                //!< Creator structure for the NVIDIA API.
    void (*create_signature)(
        NVA081_CTRL_VGPU_INFO*
    );                                    //!< Function to generate a signature.
    uint8_t ignore_version_check;         //!< If we care about version mismatch.
};

#ifdef __cplusplus
};
#endif

#endif
