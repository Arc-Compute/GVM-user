/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
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

//! IOCTL command to allocate an OS event.
#define NV_ALLOC_OS_EVENT _IOWR(NV_IOCTL_MAGIC, NV_ESC_ALLOC_OS_EVENT, nv_ioctl_alloc_os_event_t)

//! IOCTL command to free a resource.
#define NV_FREE_RES _IOWR(NV_IOCTL_MAGIC, NV_ESC_RM_FREE, NVOS00_PARAMETERS)

//! IOCTL command to control a resource.
#define NV_CTRL_RES _IOWR(NV_IOCTL_MAGIC, NV_ESC_RM_CONTROL, NVOS54_PARAMETERS)

#ifdef __cplusplus
};
#endif

#endif
