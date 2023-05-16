/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef UTILS_GPUS_H
#define UTILS_GPUS_H

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief GPU Equality Function.
 *
 * This function is used to determine if 2 GPUs are "equal".
 *
 * \param gpu - Real GPU structure to determine if it matches the criteria.
 * \param rst - What we restrict the GPU selection on.
 * \return True if the GPU matches the required restriction, False otherwise.
 */
static inline uint8_t valid_gpu(
    struct Gpu *gpu, struct Gpu *rst
)
{
    return
        (rst->domain == 0xFFFFFFFF ||
         rst->domain == gpu->domain) &&
        (rst->bus == 0xFFFFFFFF ||
         rst->bus == gpu->bus) &&
        (rst->slot == 0xFFFFFFFF ||
         rst->slot == gpu->slot) &&
        (rst->function == 0xFFFFFFFF ||
         rst->function == gpu->function) &&
        (rst->vendor_id == 0xFFFFFFFF ||
         rst->vendor_id == gpu->vendor_id) &&
        (rst->device_id == 0xFFFFFFFF ||
         rst->device_id == gpu->device_id) &&
        (rst->sub_vendor_id == 0xFFFFFFFF ||
         rst->sub_vendor_id == gpu->sub_vendor_id) &&
        (rst->sub_device_id == 0xFFFFFFFF ||
         rst->sub_device_id == gpu->sub_device_id) &&
        (rst->identifier == 0xFFFFFFFF ||
         rst->identifier == gpu->identifier) &&
        (rst->number == 0xFFFFFFFF ||
         rst->number == gpu->number);
}

#ifdef __cplusplus
};
#endif

#endif
