/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_NVIDIA_INIT_H
#define GPU_NVIDIA_INIT_H

#include <gpu/nvidia/resources.h>

#include <gpu/init.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Initial structure for NVIDIA GPUs.
 *
 * A NVIDIA GPU initialization data can have the following information.
 */
struct NvInitData {
    int ctl_fd;                              //!< nvidiactl file descriptor.
    struct NvResource *root;                 //!< Root device of the NVIDIA object.
    uint32_t (*device_ids)(
        uint32_t, uint32_t, uint32_t
    );                                       //!< Device IDs function.
    uint32_t share_type;                     //!< Type of sharing.
    uint8_t ignore_version_check;            //!< If we check on mismatch version or not.
    struct AttachedGpus *gpus;               //!< List of attached GPUs to clean in stop cmd.
};

/*! \brief Internal Structure to help with GPU info.
 */
struct NvGpu {
    int fd;                                  //!< NVIDIA file descriptor for the GPU.
    struct NvResource *dev;                  //!< GPU specific device (NV0080 class obj).
    struct NvResource *sub_dev;              //!< GPU specific device (NV2080 class obj).
    struct NvResource *engines[1024];        //!< Arbitrary limit on number of GPU engines.
};

#ifdef __cplusplus
};
#endif

#endif
