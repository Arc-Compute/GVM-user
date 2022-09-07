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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NV0080_H
#define GPU_NVIDIA_RESMAN_CLASSES_NV0080_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command to allocate a Nv0080 device
#define NV0080_CLASS 0x00000080

/*! \brief Allocates a Device
 *
 * This acts as a parameter into a method that provides us GPU specific information.
 */
struct Nv0080AllocParams {
    uint32_t deviceId;                   //!< Device ID.
    uint32_t hClientShare;               //!< Client share flag.
    uint32_t hTargetClient;              //!< Target client.
    uint32_t hTargetDevice;              //!< Target device.
    uint32_t flags;                      //!< Flags for the allocation.
    uint64_t vaSpaceSize;                //!< Space size for the virtual address space.
    uint8_t* vaStartInternal;            //!< Pointer for the start of the virtual
                                         //!< address space.
    uint64_t vaLimitInternal;            //!< Internal limit for the virtual address
                                         //!< space.
    uint32_t vaMode;                     //!< Virtual address mode.
};

#ifdef __cplusplus
};
#endif

#endif
