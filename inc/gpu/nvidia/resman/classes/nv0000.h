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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NV0000_H
#define GPU_NVIDIA_RESMAN_CLASSES_NV0000_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command for rm control res to get a list of all probed ids.
#define NV0000_GET_PROBED_IDS 0x00000214

/*! \brief Gets a list of all probed Ids.
 *
 * This acts as a parameter into a method which defines for us the different probed ids.
 */
struct Nv0000CtrlGpuGetProbedIdsParams {
    uint32_t gpu_ids[32];               //!< GPU IDs available.
    uint32_t excluded_gpus[32];         //!< GPU IDs that were excluded.
};

//! Command for rm control res to get pci info for the gpu id.
#define NV0000_GET_PCI_INFO 0x0000021B

/*! \brief Gets the PCI info for a given gpu.
 *
 * This acts as a parameter into a method which gives us the PCI info for a GPU.
 */
struct Nv0000CtrlGpuGetPciInfoParams {
    uint32_t gpu_id;                    //!< GPU Id to get PCI info from.
    uint32_t domain;                    //!< Domain for the GPU.
    uint16_t bus;                       //!< Bus for the GPU.
    uint16_t slot;                      //!< Slot for the GPU.
};

//! Command to attach a gpu id to the driver.
#define NV0000_ATTACH_IDS 0x00000215

/*! \brief Attaches the following ids to the GPU driver.
 *
 * This acts as a parameter into a method which attaches a GPU to the GPU driver.
 */
struct Nv0000CtrlGpuAttachIdsParams {
    uint32_t gpu_ids[32];               //!< List of GPU Ids to attach.
    uint32_t failed_gpu_id;             //!< Last failed GPU id.
};

//! Command to detach a gpu id from the driver.
#define NV0000_DEATTACH_IDS 0x00000216

/*! \brief DeAttaches the following ids to the GPU driver.
 *
 * This acts as a parameter into a method which deattaches a GPU to the GPU driver.
 */
struct Nv0000CtrlGpuDeAttachIdsParams {
    uint32_t gpu_ids[32];               //!< List of GPU Ids to detach.
};

//! Command to get the GPU Info.
#define NV0000_GET_GPU_INFO 0x00000202

/*! \brief Gets GPU specific information.
 *
 * This acts as a parameter into a method that provides us GPU specific information.
 */
struct Nv0000CtrlGpuGetIdInfoParams {
    uint32_t gpu_id;                    //!< GPU Id to get the information from.
    uint32_t flags;                     //!< Flags for the GPU.
    uint32_t dev_inst;                  //!< Device instance for the GPU.
    uint32_t sub_dev_inst;              //!< Sub device instance for the GPU.
    char* sz_name;                      //!< Name.
    uint32_t sli_status;                //!< SLI Status.
    uint32_t board_id;                  //!< Board id.
    uint32_t gpu_inst;                  //!< GPU Instance.
    uint32_t numa_id;                   //!< Numa id.
};

#ifdef __cplusplus
};
#endif

#endif
