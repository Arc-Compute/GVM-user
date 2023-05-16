/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_NVIDIA_RESOURCES_H
#define GPU_NVIDIA_RESOURCES_H

#include <utils/types.h>

#include <stdint.h>

#include <nvml.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Resource for managing NVIDIA kernel module objects.
 *
 * The NVIDIA kernel module implements it's own class based system to interact
 * with resources inside the kernel and speaking to the GPU. This is an incomplete
 * class which lacks a lot of the different classes that can be shown. We are
 * currently in the process of documenting and providing a tool for interacting
 * with the different allowed classes. This tool is available on the Arc-Compute
 * github and there is already extensive documentation efforts occuring on OpenMdev.
 */
struct NvResource {
    int fd;                     //!< Some physical resources require
                                //!< open file descriptors for the duration
                                //!< of the resource's lifecycle.
    uint32_t client;            //!< All resources require a client/root.
    uint32_t parent;            //!< Parent of the resource.
    uint32_t object;            //!< Object of the resource.
    uint32_t rm_class;          //!< Class of the resource.
    void *class_info;           //!< Class info for the resource.
    struct NvResource* next;    //!< Next child on the level.
    struct NvResource* child;   //!< Child of the resource.
};

/*! \brief This is a structure for interacting with NVML for the utilization API.
 */
struct NvUtilization {
    uint32_t num_gpus;          //!< Number of GPUs in the system.
    nvmlDevice_t devices[32];   //!< Devices for the NVML.
    nvmlPciInfo_t bus_ids[32];  //!< PCI Bus id for the GPU.
};

#ifdef __cplusplus
};
#endif

#endif
