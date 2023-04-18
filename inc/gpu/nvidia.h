/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_NVIDIA_H
#define GPU_NVIDIA_H

#include <gpu/nvidia/resources.h>

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Initializes the root to interact with all NVIDIA GPUs.
 *
 * This function handles the initialization of the tree for interacting with
 * the NVIDIA Kernel module.
 *
 * \param ignore_version - If we ignore the RM API version or not.
 *
 * \return The NvResource, or NULL in the case of a failure.
 */
struct NvResource *init_root(const uint8_t ignore_version);

/*! \brief Attaches a GPU to the system so we can interact with it.
 *
 * This function works to attach a GPU to the nvidia kernel and to allow for
 * us to issue GPU specific commands to the system.
 *
 * \param root - The root of the nvidia system.
 * \param device - Device id for the object.
 * \param subdevice - Sub device id for the object.
 * \param gpu - Gpu structure with identifier initialized.
 *
 * \return File descriptor for the new file related to the GPU.
 */
int get_gpu(struct NvResource *root, uint32_t device, uint32_t subdevice, struct Gpu* gpu);

#ifdef __cplusplus
};
#endif

#endif
