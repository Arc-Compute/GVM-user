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
    struct NvInitData *dat;               //!< Data for the NVIDIA driver.
    struct NvCreator *crt;                //!< Creator structure for the NVIDIA API.
    void (*create_signature)(
        NVA081_CTRL_VGPU_INFO*
    );                                    //!< Function to generate a signature.
    uint8_t ignore_version_check;         //!< If we care about version mismatch.
    uint8_t use_vmiop;                    //!< If we use VMIOP or not.
    const char *vmiop_plugin;             //!< Plugin to use for VMIOP.
    uint8_t running;                      //!< If we are running.
    int event_start;                      //!< Event start FD.
    int event_bind;                       //!< Event bind FD.
};

#ifdef __cplusplus
};
#endif

#endif
