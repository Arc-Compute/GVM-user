/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_APIS_H
#define GVM_APIS_H

#include <gpu/utilization.h>

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

//! Utilization
static struct UtilizationAPI *UTIL_APIS[TotalGpus] = {
    &nvidia_utilization_api,
    NULL
};

#ifdef __cplusplus
};
#endif

#endif
