
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
    uint8_t (*get_request)(void*);      //!< Gets a request to start a VM.
    uint8_t (*start_vm)(
        void*, struct UUID*
    );                                  //!< Starts the VM.
    uint8_t (*stop_vm)(
        void*, struct UUID*
    );                                  //!< Starts the VM.
    uint8_t (*stop)(void*);             //!< Stops the manager from running.
};

//! NVIDIA Manager API.
extern const struct ManagerAPI nvidia_manager_api;

#ifdef __cplusplus
};
#endif

#endif
