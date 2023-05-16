/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_CREATOR_H
#define GVM_CREATOR_H

#include <stddef.h>
#include <stdint.h>

#include <utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creator API.
 *
 * This is the common API to create MDevs/GPU splits.
 */
struct CreatorAPI {
    uint8_t (*init)(void*);             //!< Initialization for the creator.
    uint8_t (*create_mdevs)(
        void*,
        struct MDevRequest*, size_t,
        struct Gpu*, size_t
    );                                  //!< Creates a list of MDEVs on the system.
    uint8_t (*register_mdevs)(void*);   //!< Registers the MDEVs on the system.
    uint8_t (*stop)(void*);             //!< Stops the creator from running.
};

//! NVIDIA Creator API.
extern const struct CreatorAPI nvidia_creator_api;

#ifdef __cplusplus
};
#endif

#endif
