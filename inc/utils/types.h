/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief UUID Data structure.
 *
 * This is just the UUID data structure we use.
 */
struct UUID {
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t clock_seq_hi_and_reserved;
    uint8_t clock_seq_low;
    uint8_t node[6];
};

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
    uint32_t number;              //!< Number for the GPU (gpu0, gpu1, gpu2...).
    uint64_t free_fb;             //!< Free framebuffer on the GPU.
    uint64_t total_fb;            //!< Total framebuffer on the GPU.
    void *vendor_info;            //!< Vendor specific information.
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
