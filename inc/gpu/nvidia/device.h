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
#ifndef GPU_NVIDIA_DEVICE_H
#define GPU_NVIDIA_DEVICE_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Opens a Nvidia device.
 *
 * Opens a nvidia device given information.
 *
 * \param major - Major for the NVIDIA component to use.
 * \param minor - Minor for the NVIDIA component to use.
 * \param path - Path for the component.
 * \param proc_path - Path for the driver params.
 * \return File descriptor for the NVIDIA component.
 */
int nv_open(
    int32_t major,
    uint16_t minor,
    const char* path,
    const char* proc_path
);

/*! \brief Opens a traditional NVIDIA device.
 *
 * Opens a traditional NVIDIA device given a minor number.
 *
 * \param minor - Minor to use to open the device.
 * \return File descriptor for the traditional NVIDIA component.
 */
int nv_open_dev(uint16_t minor);

/*! \brief Opens a Mediated NVIDIA device.
 *
 * Opens a Mediated NVIDIA device given a minor number.
 *
 * \param minor - Minor to use to open the device.
 * \return File descriptor for the Mediated NVIDIA component.
 */
int nv_open_mdev(uint16_t minor);

#ifdef __cplusplus
};
#endif

#endif
