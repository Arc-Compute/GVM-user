/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <gpu/nvidia/device.h>
#include <gpu/nvidia/init.h>
#include <gpu/nvidia/resources.h>
#include <gpu/nvidia/resman.h>

#include <gvm/apis.h>
#include <gvm/creator.h>
#include <gvm/nvidia/creator.h>
#include <gvm/nvidia/manager.h>
#include <gvm/nvidia/debug/envy.h>

#include <utils/gpus.h>

#include <class/cla081.h>

#include <ctrl/ctrla081.h>

#include <rs_access.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0

//! Initialization API.
#define initializer INIT_APIS[Nvidia]

//! Creator API.
#define creator CREATOR_APIS[Nvidia]

/*! \brief Creator device id codebase.*/
static uint32_t manager_ids(
    uint32_t client, uint32_t gpu_id, uint32_t handle
)
{
    return ((client + gpu_id) << 8) | (handle & ((1 << 8) - 1));
}

/*! \brief Initialization Code for NVIDIA.
 *
 * This code initializes the NVIDIA GVM system for manager.
 */
static uint8_t init(void *in)
{
    return 0;
}

/*! \brief Stops the NVIDIA Manager API.
 */
static uint8_t stop(void *in)
{
    return 0;
}

const struct ManagerAPI nvidia_manager_api = {
.init = init,
.create_mdevs = create_mdevs,
.register_mdevs = register_mdevs,
.stop = stop
};

#endif
