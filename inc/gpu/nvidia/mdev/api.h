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
#ifndef GPU_NVIDIA_MDEV_API_H
#define GPU_NVIDIA_MDEV_API_H

#include <stdio.h>
#include <stdint.h>

#include <gpu/nvidia/mdev/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creates a mediated device event.
 *
 * This is a function to create a MDEV event which can be used by the
 * VM in order to create the necessary events.
 *
 * \param event_type - Type of the event to create.
 * \param id - Id of the mdev instance to use.
 * \return File descriptor for the mdev event.
 */
int nv_mdev_create_event(uint32_t event_type, uint16_t id);

#ifdef __cplusplus
};
#endif

#endif
