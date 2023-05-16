/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <gvm/apis.h>

const struct InitializationAPI *INIT_APIS[TotalGpus] = {
    &nvidia_initialization_api,
    NULL
};

const struct UtilizationAPI *UTIL_APIS[TotalGpus] = {
    &nvidia_utilization_api,
    NULL
};

const struct CreatorAPI *CREATOR_APIS[TotalGpus] = {
    &nvidia_creator_api,
    NULL
};
