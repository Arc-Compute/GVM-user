/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef UTILS_DEVICE_H
#define UTILS_DEVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Does a device file exist?
 *
 * This function provides us information regarding if a device exists
 * on the system, it automatically prepends the /dev/ prefix if the prefix
 * is not found in the string.
 *
 * \sideeffect File System Side Effect: This checks the file system,
 *                                      specifically the device tree.
 *
 * \param device_name - The name of the device that we are checking if it
 *                      exists or not in the /dev/ filesystem.
 * \return Returns a 0 if the device does not exist, and a 1 if it does.
 */
uint8_t device_exists(const char* device_name);

/*! \brief Gets the device major id from /proc/devices
 *
 * This function is used to provide us information regarding what is the major
 * number for a particular for a device.
 *
 * \sideeffect File System Side Effect: Checks the major number of a device.
 *
 * Example
 * =======
 * ```{.c}
 * get_major("nvidia-frontend") == 195
 * ```
 *
 * \param device - Device we want to create.
 * \return Returns the major number related to the device, or -1 in failure.
 */
int32_t get_major(const char* device);

/*! \brief Gets a parameter from a driver param file.
 *
 * This function is designed to give us several parameters by which we can
 * determine how to create the necessary character devices.
 *
 * \sideeffect File System Side Effect: Checks the device parameter file for
 *                                      a value.
 *
 * \restriction This function will only work for integer parameters.
 *
 * Example
 * =======
 * ```{.c}
 * get_param("/proc/driver/nvidia/params", "DeviceFileUID") == 0
 * get_param("/proc/driver/nvidia/params", "DeviceFileGID") == 0
 * get_param("/proc/driver/nvidia/params", "ResmanDebugLevel") == 4294967295
 * ```
 *
 * \param filename - File name for the parameter file.
 * \param parameter - Driver parameter to search for.
 * \return Returns the integer parameter for the driver parameter we searched
 *         for, or -1 if not found.
 */
int32_t get_param(const char* filename, const char* parameter);

#ifdef __cplusplus
};
#endif

#endif
