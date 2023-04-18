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
};

#ifdef __cplusplus
};
#endif

#endif
