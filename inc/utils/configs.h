/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#include <utils/types.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Structure to handle GPU Configurations.
 *
 * This structure works on implementing the GPU configurations necessary to generate the Mediated GPUs.
 */
struct GpuConfig {
    struct Gpu* gpus;              //!< List of gpus.
    size_t gpu_size;               //!< How many gpus are in the list of gpus.
    struct MDevRequest* requests;  //!< List of requested mdevs.
    size_t mdev_size;              //!< How many requested mdevs are in the list of mdevs.
};

/*! \brief Structure to provide a list of GPU configurations.
 *
 * This provides the GPU Configurations as well as a size of GPU configs.
 */
struct GpuConfigs {
    struct GpuConfig* configs;     //!< List of configurations.
    size_t config_size;            //!< Size of the configuration list.
};

/*! \brief Parses a config file.
 *
 * This code parses a config file and produces a GpuConfig object.
 *
 * \param name - Name of the config file.
 * \return A gpu config file, which tells us what mdevs to create.
 */
struct GpuConfigs get_configs(const char* name);

#ifdef __cplusplus
};
#endif

#endif
