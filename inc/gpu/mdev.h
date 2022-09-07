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
#ifndef GPU_MDEV_H
#define GPU_MDEV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief GPU Base Structure.
 *
 * This structure is a bare bones GPU that gives us some information over how
 * the physical connected GPU is.
 */
struct Gpu {
    uint32_t domain;              //!< Domain for the PCI device.
    uint32_t bus;                 //!< Bus for the PCI device.
    uint32_t slot;                //!< Slot for the PCI device.
    uint32_t function;            //!< Function for the PCI device.
    uint32_t vendor_id;           //!< Vendor id for the PCI device.
    uint32_t device_id;           //!< Device id for the PCI device.
    uint32_t sub_vendor_id;       //!< Sub vendor id for the PCI device.
    uint32_t sub_device_id;       //!< Sub device id for the PCI device.
    uint32_t identifier;          //!< Identifier for the GPU.
};

/*! \brief Virtual Display.
 *
 * Structure for creating a virtual display.
 */
struct VirtDisplay {
    uint32_t num_heads;           //!< Number of monitor heads.
    uint32_t max_res_x;           //!< Max resolution X.
    uint32_t max_res_y;           //!< Max resolution Y.
    uint32_t frl_config;          //!< Frame rate limiter.
    uint32_t frl_enable;          //!< If we use the frame rate limiter.
};

/*! \brief Mediated Device Request Structure.
 *
 * MDev Request structure to convert from an internal structure to
 * a mediated device.
 */
struct MDevRequest {
    uint32_t num;                 //!< Number of the mdev.
    uint64_t v_dev_id;            //!< Virtual device id.
    uint64_t p_dev_id;            //!< Physical device id.
    const char* name;             //!< Name of the GPU.
    const char* gpu_class;        //!< GPU Class structure.
    uint32_t max_inst;            //!< Max number of mediated GPUs.
    struct VirtDisplay* disp;     //!< Virtual display structure.
    uint8_t ecc_support;          //!< If the Mediated GPU has ECC supported.
    uint8_t multi_mdev;           //!< If multiple mdevs supported.
    uint32_t fb_len;              //!< Frame buffer length (IN MEGABYTES).
    uint32_t fb_res;              //!< Frame buffer reserved (IN MEGABYTES).
    uint32_t map_vid_size;        //!< Mappable video size (IN MEGABYTES).
    uint32_t enc_cap;             //!< What percentage of the encoder will be
                                  //!< allocated to this mediated device.
    uint32_t bar1_len;            //!< Bar 1 Length.
};

#ifdef __cplusplus
};
#endif

#endif
