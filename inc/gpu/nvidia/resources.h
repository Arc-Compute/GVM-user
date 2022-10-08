/*
 * Copyright (C) 2022 2666680 Ontario Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef GPU_NVIDIA_RESOURCES_H
#define GPU_NVIDIA_RESOURCES_H

#include <gpu/mdev.h>

#include <stdint.h>

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
    uint64_t object;            //!< Object of the resource.
    uint32_t rm_class;          //!< Class of the resource.
    void *class_info;           //!< Class info for the resource.
    struct NvResource* next;    //!< Next child on the level.
    struct NvResource* child;   //!< Child of the resource.
};

/*! \brief Control Mechanism for the NVIDIA GPU.
 *
 * Mediated GPU controller for the Nvidia GPU structure.
 */
struct NvMdevGpu {
    int ctl_fd;                 //!< Control Nvidia control file description.
    int dev_fd;                 //!< Device Nvidia file descriptor.
    int mdev_fd;                //!< Mdev file descriptor.
    struct Gpu* gpu;            //!< GPU structure corresponding to the GPU.
    uint32_t root;              //!< Initial client.
    uint32_t device;            //!< Device id for controlling the physical gpu.
    uint32_t sub_device;        //!< Sub device id.
    uint32_t mdev_config;       //!< Configurator for mdev devices.
    struct NvResource* dev;     //!< Device.
    struct NvResource* sdev;    //!< Subdevice.
    struct NvResource* mdev;    //!< Mdev device.
};

/*! \brief Structure for managing the mediated stack.
 *
 * Structure for managing the entire mediated GPU structure for Nvidia.
 */
struct NvMdev {
    int fd;                     //!< Control file descriptor.
    struct NvMdevGpu* gpus[32]; //!< Available GPUs.
    struct NvResource* res;     //!< Resource tree.
};

#ifdef __cplusplus
};
#endif

#endif
