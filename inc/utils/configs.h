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
#ifndef UTILS_CONFIGS_H
#define UTILS_CONFIGS_H

#include <gpu/mdev.h>

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
