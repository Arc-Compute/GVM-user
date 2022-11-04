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
#ifndef GVM_NVIDIA_INIT_H
#define GVM_NVIDIA_INIT_H

#include <gpu/nvidia/resources.h>

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief This structure is for handling a VM manager specifically for a
 *         NVIDIA system.
 *
 * We built this to provide control regarding a VM for a NVIDIA based GPU.
 */
struct NvVmMgr {
    uint64_t *guest_cache;       //!< Guest PFN to Machine FN cache.
    uint16_t *guest_ref_cache;   //!< Guest PFN reference count array.
    uint32_t max_pfn_count;      //!< Initialize to (512 * 1024).
    struct NvResource* vm;       //!< VM resource.
    struct NvMdevGpu* mdev_gpu;  //!< Mdev GPU.
};

/*! \brief Reverse engineered start vm info structure.
 *
 * This structure is used to provide us information regarding how
 * to get the VM information.
 */
struct RmVmStartInfo {
    struct UUID uuid;            //!< UUID for the MDEV.
    char config[1024];           //!< Config for starting the MDEV.
    uint32_t qemu_pid;           //!< QEMU PID.
    uint32_t pci_id;             //!< PCI id.
    uint16_t mdev_id;            //!< MDEV id.
    uint32_t pci_bdf;            //!< BDF PCI.
};

/*! \brief Notifies the start of a VM.
 *
 * This structure is used to notify the starting of a VM's
 */
struct RmVmNotifyStart {
    struct UUID mdev;            //!< MDEV UUID.
    struct UUID vm;              //!< VM UUID.
    char name[128];              //!< VM Name.
    uint32_t status;             //!< Status for the notified start.
};

#ifdef __cplusplus
};
#endif

#endif
