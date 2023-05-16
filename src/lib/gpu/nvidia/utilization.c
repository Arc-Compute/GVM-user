/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <gpu/nvidia/resources.h>
#include <gpu/utilization.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <nvml.h>

static void init(void *in)
{
    struct NvUtilization *util = ((struct NvUtilization*) in);
    nvmlInit();

    if (in == NULL)
        return;

    nvmlDeviceGetCount_v2(&(util->num_gpus));

    for (uint32_t i = 0; i < util->num_gpus; ++i) {
        nvmlDeviceGetHandleByIndex_v2(i, &(util->devices[i]));
        nvmlDeviceGetPciInfo_v3(util->devices[i], &(util->bus_ids[i]));
    }
}

static struct AvgGPUUtils *sample(void *in)
{
    struct NvUtilization *util = ((struct NvUtilization*) in);
    struct AvgGPUUtils *ret = calloc(1, sizeof(struct AvgGPUUtils));

    if (ret == NULL)
        return NULL;

    ret->num_gpus = util->num_gpus;

    for (uint32_t i = 0; i < util->num_gpus; ++i) {
        nvmlUtilization_t rates = {};
        nvmlMemory_t memory = {};

        nvmlDeviceGetUtilizationRates(util->devices[i], &rates);
        nvmlDeviceGetMemoryInfo(util->devices[i], &memory);

        ret->array[i].bus_id = util->bus_ids[i].busId;
        ret->array[i].compute_contexts = (float) rates.gpu;
        ret->array[i].vram_accesses = (float) rates.memory;
        ret->array[i].vram_alloted = ((float) memory.used) / ((float) memory.total);
        ret->array[i].vram_alloted *= 100.0;
    }

    return ret;
}

static void stop(void *in __attribute__((unused)))
{
    nvmlShutdown();
}

const struct UtilizationAPI nvidia_utilization_api = {
.init = init,
.sample = sample,
.stop = stop
};
