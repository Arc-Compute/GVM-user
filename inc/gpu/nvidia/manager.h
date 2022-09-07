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
#ifndef GPU_NVIDIA_MANAGER_H
#define GPU_NVIDIA_MANAGER_H

#include <gpu/mdev.h>

#include <gpu/nvidia/resources.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creates a NVIDIA manager object.
 *
 * This function initalizes the manager object for the NVIDIA GPU.
 *
 * \sideeffect File System Side Effect: Can potentially create a few /dev files.
 * \sideeffect File System Side Effect: Opens /dev/nvidiactl.
 * \sideeffect File System Side Effect: Opens /dev/nvidia%d for different GPUs.
 * \sideeffect RM Side Effect: Creates a RM Client.
 * \sideeffect RM Side Effect: Attaches a GPU to /dev/nvidia%d.
 * \sideeffect RM Side Effect: Creates a RM Device Id.
 * \sideeffect RM Side Effect: Creates a RM Sub Device Id.
 * \sideeffect RM Side Effect: Creates a mdev configurator id.
 *
 * \return A manager for the NVIDIA system.
 */
struct NvMdev create_nv_mgr();

/*! \brief Deletes a NVIDIA manager object.
 *
 * This function destroys the manager object for the NVIDIA GPU.
 *
 * \sideeffect RM Side Effect: Deletes a RM Client.
 * \sideeffect RM Side Effect: DeAttaches a GPU from /dev/nvidia%d.
 * \sideeffect RM Side Effect: Deletes a RM Device Id.
 * \sideeffect RM Side Effect: Deletes a RM Sub Device Id.
 * \sideeffect RM Side Effect: Deletes a mdev configurator id.
 *
 * \param mgr - Pointer to the manager structure to free.
 */
void free_nv_mgr(struct NvMdev *mgr);

/*! \brief Creates necessary mediated devices on GPUs.
 *
 * Creates mediated devices on the devices.
 *
 * \sideeffect RM Side Effect: Creates mdevs on a requested GPU.
 *
 * \param mgr - Pointer to the manager for the NVIDIA driver.
 * \param limited - GPUs to limit the requested drivers on.
 * \param gpu_size - Size of the limited list.
 * \param requested - Requested MDevs to use.
 * \param mdev_size - Size of the requested list.
 */
void create_nv_mgr_mdevs(
    struct NvMdev *mgr,
    struct Gpu* limited,
    size_t gpu_size,
    struct MDevRequest* requested,
    size_t mdev_size
);

/*! \brief Registers mdevs on the OS.
 *
 * This command actually registers the MDevs in the Operating System.
 *
 * \sideeffect RM Side Effect: Creates mdevs in the OS.
 *
 * \param mgr - Manager to register the MDevs for.
 */
void register_nv_mgr_mdevs(struct NvMdev *mgr);

#ifdef __cplusplus
};
#endif

#endif
