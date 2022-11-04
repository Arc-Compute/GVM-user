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
#ifndef GPU_INTEL_MANAGER_H
#define GPU_INTEL_MANAGER_H

#include <gpu/mdev.h>

#include <gpu/intel/resources.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creates an INTEL manager object.
 *
 * This function initalizes the manager object for the INTEL GPU.
 *
 * \sideeffect File System Side Effect: Can potentially create a few /dev files.
 *
 * \return A manager for the Intel system.
 */
struct IntelMdev create_intel_mgr();

/*! \brief Deletes an INTEL manager object.
 *
 * This function destroys the manager object for the INTEL GPU.
 *
 * \param mgr - Pointer to the manager structure to free.
 */
void free_intel_mgr(struct IntelMdev *mgr);

/*! \brief Creates necessary mediated devices on INTEL GPUs.
 *
 * Creates mediated devices on the devices.
 *
 * \param mgr - Pointer to the manager for the INTEL driver.
 * \param limited - GPUs to limit the requested drivers on.
 * \param gpu_size - Size of the limited list.
 * \param requested - Requested MDevs to use.
 * \param mdev_size - Size of the requested list.
 */
void create_intel_mgr_mdevs(
    struct IntelMdev *mgr,
    struct Gpu* limited,
    size_t gpu_size,
    struct MDevRequest* requested,
    size_t mdev_size
);

#ifdef __cplusplus
};
#endif

#endif
