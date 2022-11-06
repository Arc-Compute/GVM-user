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
#ifndef GPU_MANAGER_H
#define GPU_MANAGER_H

#include <gpu/intel/manager.h>
#include <gpu/nvidia/resources.h>

#include <utils/configs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Type of Mediated Device Manager to use.
 */
enum MdevType {
    INVALID,         //!< Invalid MdevMgr
    NVIDIA,          //!< NVIDIA specific MdevMgr
    INTEL            //!< INTEL specific MdevMgr
};

/*! \brief Mediated Device Manager object that is common between Intel, NVIDIA,
 *         and more.
 *
 * This class handles all forms of mediated device types.
 */
struct MdevMgr {
    enum MdevType type;  //!< Type of the MdevMgr to use.
    struct NvMdev nvidia;
    struct IntelMdev intel;
};

/*! \brief Creates a mediated device manager.
 *
 * Creates the necessary mdev manager.
 *
 * \param configs - Configurations to set.
 * \return Manager for the MDev.
 */
struct MdevMgr create_mdev_mgr(struct GpuConfigs configs);

/*! \brief Frees the mediated device manager.
 *
 * Deletes the mediated device manager.
 *
 * \param mgr - Pointer to manager object.
 * \return Performed for side effects.
 */
void free_mdev_mgr(struct MdevMgr *mgr);

#ifdef __cplusplus
};
#endif

#endif
