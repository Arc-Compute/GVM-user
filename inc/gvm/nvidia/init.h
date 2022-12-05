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
#include <gpu/nvidia/resman/classes/nv2080.h>

#include <utils/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief FB Info.
 *
 * This is the structure for the guest FB info.
 */
struct NvFBInfo {
    uint32_t bank_sizzle_align;
    uint32_t fb_len_mbs;
    uint32_t fb_bus_width;
    uint32_t ram_type;
    uint32_t fbio_mask;
    uint32_t fbp_count;
    uint32_t fbp_mask;
    uint32_t l2_cache_size;
    uint32_t comptag_1to1_enabled;
    uint32_t page_retirement_enabled;
    uint32_t poison_fuse_enabled;
    uint32_t fbpa_ecc_vbios_enabled;
    uint32_t dynamic_blacklist_enabled;
    uint32_t ltc_mask;
    uint32_t lts_count;
};

/*! \brief This structure is for handling a VM manager specifically for a
 *         NVIDIA system.
 *
 * We built this to provide control regarding a VM for a NVIDIA based GPU.
 */
struct NvVmMgr {
    uint64_t *guest_cache;          //!< Guest PFN to Machine FN cache.
    uint16_t *guest_ref_cache;      //!< Guest PFN reference count array.
    uint32_t max_pfn_count;         //!< Initialize to (512 * 1024).
    uint32_t mdev_id;               //!< MDEV device id.
    uint32_t mdev_dev_id;           //!< MDEV nvidia id.
    uint32_t mdev_dev_id2;          //!< MDEV nvidia id2.
    uint32_t fb_device;             //!< Device handle for the framebuffer.
    uint32_t fb_length;             //!< Frame buffer length presented into the guest.
    uint32_t mdev_caps;             //!< Mediated capabilities.
    uint32_t writeable_mdev_caps;   //!< Mediated capabilities.
    uint64_t max_mfn;               //!< Maximum MFN supported.
    uint32_t fifo_channel_count;    //!< FIFO Physical Channel Count.
    uint32_t gpu_fb_mbs;            //!< Framebuffer size in MBs.
    uint8_t gr_caps[23];            //!< Graphics capabilities.
    uint32_t pid;                   //!< VM PID.
    struct UUID vm_uuid;            //!< VM UUID.
    struct Nv2080GpuGetGidInfo gid; //!< GID info.
    struct NvFBInfo fb_info;        //!< Guest FBs.
    struct NvResource* vm;          //!< VM resource.
    struct NvMdevGpu* mdev_gpu;     //!< Mdev GPU.
    struct NvResource* mdev_device; //!< MDEV device inside the NVIDIA module.
    struct NvResource* mdev_device2; //!< MDEV device inside the NVIDIA module.
};

/*! \brief Creatively debugged start vm info structure.
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
