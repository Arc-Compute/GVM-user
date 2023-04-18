/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_MDEV_H
#define GPU_MDEV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
