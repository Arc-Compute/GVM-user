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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NVA082_H
#define GPU_NVIDIA_RESMAN_CLASSES_NVA082_H

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command to allocate a NvA082 device
#define NVA082_CLASS 0x0000A082

/*! \brief Allocates a mdev control mechanism.
 *
 * Parameters to control mdev.
 */
struct NvA082AllocParams {
    uint32_t mdev_type;
    uint32_t vm_pid;
    uint32_t unknown;
    uint32_t swizz_id;
    uint32_t num_channels;
    uint32_t num_plugin_channels;
    uint32_t vm_id_type;
    struct UUID vm_id;
    uint32_t smc_stuff;
    uint32_t mdev_instance_id;
};

#ifdef __cplusplus
};
#endif

#endif
