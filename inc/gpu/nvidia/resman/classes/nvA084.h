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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NVA084_H
#define GPU_NVIDIA_RESMAN_CLASSES_NVA084_H

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Configuration to interact with a mdev device allocated in the kernel.
#define NVA084_CLASS 0x0000A084

//! Maximum number of VMMU segments on the machine.
#define MAX_VMMU_SEGMENTS 384

//! Command for rm control res to set lifecycle control.
#define NVA084_SET_LIFE 0xA0840102

//! Created mdev.
#define NVA084_CREATE_MDEV 0

/*! \brief Allocates a mdev device in the kernel module.
 *
 * This structure corresponds to the allocation of a mdev host device inside the kernel.
 */
struct NvA084AllocParams {
    uint32_t dbdf;                              //!< Domain (31:16), bus (15:8), device (7:3), function (2:0).
    uint32_t sriov_id;                          //!< Only used when SR-IOV is used.
    uint32_t mig_id;                            //!< Only used when MIG is used, set to 0xFFFFFFFF otherwise.
    uint32_t mdev_type;                         //!< Type of the MDEV config to use.
    uint32_t vm_pid;                            //!< PID of the VM.
    uint32_t num_channels;                      //!< Number of channels allowed in the guest (only with SR-IOV).
    uint32_t num_psuedo_channels;               //!< Number of psuedo channels into the guest.
    uint32_t weird_stuff;                       //!< 0 or 1, mostly 1 (Seems to be related to XEN).
    struct UUID uuid_or_magic;                  //!< UUID if weird stuff is 1 (Potentially magic is XEN Doms).
    uint32_t magic_smc_crap;                    //!< Unknown magic 0 or 1 related to the SMC.
    uint32_t mdev_dev_id;                       //!< MDEV instance ID.
    uint32_t num_fb_handles;                    //!< Number of FB handles.
    uint32_t guest_fb_list[MAX_VMMU_SEGMENTS];  //!< List of FB handles.
    uint32_t heap_memory;                       //!< Internal heap memory.
    uint64_t ctrl_offset;                       //!< Control buffer offset.
};

struct NvA084SetMdev {
    struct UUID uuid;
    uint32_t mdev_dev_id;
};

#ifdef __cplusplus
};
#endif

#endif
