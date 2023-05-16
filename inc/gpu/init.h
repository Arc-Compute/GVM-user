/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_INIT_H
#define GPU_INIT_H

#include <stdint.h>

#include <utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief A list of attached GPUs.
 *
 * This is the list of the attached GPUs on this particular node.
 */
struct AttachedGpus {
    uint32_t num_gpus;                        //!< Number of GPUs that are attached on node.
    struct Gpu gpus[32];                      //!< Maximum of 32 GPUs supported.
};

/*! \brief Initialization API.
 *
 * This is a common API that other APIs may make use of to structure the
 * individual GPU information.
 */
struct InitializationAPI {
    uint8_t (*init)(void*);                   //!< Initialization data for the "ROOT" object.
    struct AttachedGpus *(*get_gpus)(void*);  //!< Attaches and gets the gpus.
    uint8_t (*get_used_fb)(
        void*, struct Gpu*
    );                                        //!< Get the currently used frame buffer.
    uint8_t (*stop)(void*);                   //!< Mechanism to stop the "ROOT" object.
};

//! NVIDIA Initialization API.
extern const struct InitializationAPI nvidia_initialization_api;

#ifdef __cplusplus
};
#endif

#endif
