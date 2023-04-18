/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
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
