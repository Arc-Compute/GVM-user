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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NV2080_H
#define GPU_NVIDIA_RESMAN_CLASSES_NV2080_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command to allocate a Nv2080 device
#define NV2080_CLASS 0x00002080

//! Command to get the BUS PCI info.
#define NV2080_GET_BUS_PCI_INFO 0x20801801

/*! \brief Gets the BUS PCI info.
 *
 * This acts as a parameter into a method which gives us the BUS PCI info for a GPU.
 */
struct BusGetPciInfo {
    uint32_t dev_id;                 //!< Device ID.
    uint32_t sub_dev_id;             //!< Sub device ID.
    uint32_t revision_id;            //!< Revision ID.
    uint32_t ext_id;                 //!< Extension ID.
};

#ifdef __cplusplus
};
#endif

#endif
