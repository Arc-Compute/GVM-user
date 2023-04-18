/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_UTILIZATION_H
#define GPU_UTILIZATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Average GPU Utilization.
 *
 * Structure to provide a common Average GPU Utilization structure.
 *
 * TODO: Expand to handle encoders and decoder and different types of GPU cores.
 */
struct AvgGPUUtil {
    const char *bus_id;            //!< Bus id for the GPU.
    float compute_contexts;        //!< Percent of the number of compute contexts active.
    float vram_accesses;           //!< Percent of the number of global reads/writes.
    float vram_alloted;            //!< Percent of vRAM already alloted.
};

/*! \brief Unified return structure to say the utilization across all GPUs.
 *
 * This structure is a wrapper to help simplify the return structure.
 */
struct AvgGPUUtils {
    uint32_t num_gpus;             //!< Number of GPUs of this kind in the system.
    struct AvgGPUUtil array[32];   //!< Maximum of 32 GPUs supported.
};

/*! \brief Utilization Control API
 *
 * This is a common API that all GPUs that need to be implemented in order
 * for us to support GPU Utilization.
 */
struct UtilizationAPI {
    void (*init)(void*);                  //!< Initializes a utilization system.
    struct AvgGPUUtils *(*sample)(void*); //!< Provide a list of samples GPU utilization.
    void (*stop)(void*);                  //!< Shuts down the utilization system.
};

//! NVIDIA Utilization API.
extern struct UtilizationAPI nvidia_utilization_api;

#ifdef __cplusplus
};
#endif

#endif
