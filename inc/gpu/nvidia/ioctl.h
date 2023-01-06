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
#ifndef GPU_NVIDIA_IOCTL_H
#define GPU_NVIDIA_IOCTL_H

#include <sys/ioctl.h>

#include <nv_escape.h>
#include <nv-ioctl.h>
#include <nvos.h>

#ifdef __cplusplus
extern "C" {
#endif

//! IOCTL command to perform a version check on the system.
#define NV_VERSION_CHECK _IOWR(NV_IOCTL_MAGIC, NV_ESC_CHECK_VERSION_STR, nv_ioctl_rm_api_version_t)

//! IOCTL command to allocate a resource.
#define NV_ALLOC_RES _IOWR(NV_IOCTL_MAGIC, NV_ESC_RM_ALLOC, NVOS21_PARAMETERS)

//! IOCTL command to free a resource.
#define NV_FREE_RES _IOWR(NV_IOCTL_MAGIC, NV_ESC_RM_FREE, NVOS00_PARAMETERS)

//! IOCTL command to control a resource.
#define NV_CTRL_RES _IOWR(NV_IOCTL_MAGIC, NV_ESC_RM_CONTROL, NVOS54_PARAMETERS)

#ifdef __cplusplus
};
#endif

#endif
