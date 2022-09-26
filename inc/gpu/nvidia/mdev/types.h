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
#ifndef GPU_NVIDIA_MDEV_TYPES_H
#define GPU_NVIDIA_MDEV_TYPES_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief MDEV IOCTL message for nvidia.
 *
 * NVIDIA requires this format to send IOCTL commands to MDEV device.
 */
struct NvMdevIoctl {
    void* params;           //!< Parameter to send into the kernel module.
    uint32_t status;        //!< Resulting status of IOCTL.
    uint32_t size;          //!< Size of the parameter.
};

#ifdef __cplusplus
};
#endif

#endif
