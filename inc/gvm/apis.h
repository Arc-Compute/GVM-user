/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_APIS_H
#define GVM_APIS_H

#include <gpu/init.h>
#include <gpu/utilization.h>

#include <gvm/creator.h>
#include <gvm/manager.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! These are the GPUs we currently support in GVM.
 */
enum SupportedGpus {
    Nvidia = 0,                   //!< NVIDIA based GPUs.
    Intel = 1,                    //!< INTEL based GPUs (CODE REMOVED FOR NOW).
    TotalGpus = 2                 //!< Number of supported GPUs inside the GVM system.
};

//! Initialization APIs
extern const struct InitializationAPI *INIT_APIS[TotalGpus];

//! Utilization APIs
extern const struct UtilizationAPI *UTIL_APIS[TotalGpus];

//! Creator APIs
extern const struct CreatorAPI *CREATOR_APIS[TotalGpus];

//! Manager APIs
extern const struct ManagerAPI *MANAGER_APIS[TotalGpus];

#ifdef __cplusplus
};
#endif

#endif
