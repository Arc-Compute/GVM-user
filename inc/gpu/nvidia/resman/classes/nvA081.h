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
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NVA081_H
#define GPU_NVIDIA_RESMAN_CLASSES_NVA081_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command to allocate a NvA081 device
#define NVA081_CLASS 0x0000A081

//! Command for rm control res to add a mdev to the list of mdevs.
#define NVA081_ADD_MDEV 0xA0810101

//! Command for rm control res to notify start for a VM.
#define NVA081_NOTIFY_START 0xA0810107

//! Command for rm control res to register mdevs.
#define NVA081_REG_MDEV 0xA0810109

/*! \brief Creates an MDev Config to be sent into the RM core.
 *
 * This structure is a parameter to create an MDev class, this needs a
 * register command afterwards to make the class available.
 */
struct RmMdevConfig {
    uint64_t discard;
    uint32_t mdev_type;
    char name[32];
    char class[32];
    char sign[128];
    char pact[132];
    uint32_t max_instances;
    uint32_t num_heads;
    uint32_t max_res_x;
    uint32_t max_res_y;
    uint32_t max_pixel;
    uint32_t frl_config;
    uint32_t cuda;
    uint32_t ecc_support;
    uint32_t gpu_instance_size;
    uint32_t multi_mdev;
    uint64_t enc_cap;
    uint64_t v_dev_id;
    uint64_t p_dev_id;
    uint64_t fb_len;
    uint64_t map_video;
    uint64_t fb_res;
    uint64_t bar1_len;
    uint32_t frl_enable;
    uint32_t mdev_extra_params[1027];
};

#ifdef __cplusplus
};
#endif

#endif
