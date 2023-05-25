
/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_MANAGER_H
#define GVM_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#include <utils/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Manager API.
 *
 * This is the common API to handle running VMs on a GPU.
 */
struct ManagerAPI {
    uint8_t (*init)(void*);             //!< Initialization for the manager.
    uint8_t (*create_mdevs)(
        void*,
        struct MDevRequest*, size_t,
        struct Gpu*, size_t
    );                                  //!< Creates a list of MDEVs on the system.
    uint8_t (*register_mdevs)(void*);   //!< Registers the MDEVs on the system.
    uint8_t (*handle_event)(void*);     //!< Handles a vGPU request.
    uint8_t (*stop)(void*);             //!< Stops the manager from running.
    uint8_t (*running)(void*);          //!< Determines if we are running the program.
};

//! NVIDIA Manager API.
extern const struct ManagerAPI nvidia_manager_api;

#ifdef __cplusplus
};
#endif

#endif
