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
#ifndef GPU_INTEL_RESOURCES_H
#define GPU_INTEL_RESOURCES_H

#include <gpu/mdev.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Control VF function for INTEL.
 *
 * This determines how to control the INTEL vGPU on runtime.
 */
struct IntelVF {
    int fn;                              //!< Function for the virtual function in the DBSF format.
    size_t fb;                           //!< Framebuffer size.
    int active;                          //!< If the VF is active or not.
    int control_fd;                      //!< Control file descriptor.
};

/*! \brief Control Mechanism for the INTEL GPU.
 *
 * Mediated GPU controller for the Intel GPU structure.
 */
struct IntelMdevGpu {
    struct Gpu gpu;                      //!< GPU structure corresponding to the GPU.
    size_t max_fb;                       //!< Maximum allocatable framebuffer.
    size_t partitions;                   //!< Number of partitions so far.
    size_t used_fb;                      //!< Framebuffer currently being used.
    size_t remaining;                    //!< Remaining number of partitions available.
    struct IntelVF *vfs[31];             //!< Maximum possible VFs on Intel is 31 VFs.
    struct MDevRequests *available[128]; //!< Available mdevs on the GPU.
};

/*! \brief Structure for managing the mediated stack.
 *
 * Structure for managing the entire mediated GPU structure for Intel.
 */
struct IntelMdev {
    struct IntelMdevGpu* gpus[32]; //!< Available GPUs, (arbitrary limit).
};

#ifdef __cplusplus
};
#endif

#endif
