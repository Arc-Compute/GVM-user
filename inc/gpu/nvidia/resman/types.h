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
#ifndef GPU_NVIDIA_RESMAN_TYPES_H
#define GPU_NVIDIA_RESMAN_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief RM API Version Check.
 *
 * NVIDIA requires a version check to ensure that the binary offsets are correct between the
 * userland program and the kernel module.
 */
struct NvVersionCheck {
    uint32_t cmd;            //!< Command override (Allows you to hid the version).
    uint32_t reply;          //!< Reply if the driver is correct or not.
    char version[64];        //!< Version for the driver.
};

/*! \brief Operating System Event Allocator.
 *
 * Type to allocate an operating system event call from the kernel.
 */
struct RmAllocOsEvent {
    uint32_t client;         //!< Client for the operating system event.
    uint32_t device;         //!< Device for the operating system event.
    int fd;                  //!< File descriptor for the operating system
                             //!< event.
    uint32_t status;         //!< Status for the allocation.
};

/*! \brief Resource Allocator.
 *
 * Type from the NVIDIA Kernel module for allocating a resource in the kernel module.
 */
struct RmAllocRes {
    uint32_t hRoot;          //!< Client allocating the object.
    uint32_t hObjectParent;  //!< Parent of the object we allocate.
    uint32_t hObjectNew;     //!< New object we allocate.
    uint32_t hClass;         //!< Class of the new object.
    void *pAllocParams;      //!< Allocation parameters.
    uint32_t status;         //!< Status regarding execution.
};

/*! \brief Allocates a RM Event.
 *
 * This is passed as a parameter into the Resource Allocator.
 */
struct RmAllocEvent {
    uint32_t parent;         //!< Parent of the event.
    uint32_t src;            //!< Source of the event.
    uint32_t event_class;    //!< Class of the event.
    uint32_t notify;         //!< Notification of the event.
    uint64_t event_data;     //!< Data for the event.
};

/*! \brief Sets the type of notification to use.
 *
 * Sets certain notification parameters.
 */
struct RmSetNotification {
    uint32_t event;          //!< Event to set the notification structure.
    uint32_t action;         //!< Action to use for the notification.
};

/*! \brief Resource Deallocator.
 *
 * Structure to free a resource.
 */
struct RmFreeRes {
    uint32_t hRoot;          //!< Client for deallocation of the object.
    uint32_t hObjectParent;  //!< Parent to deallocate the object.
    uint32_t hObjectOld;     //!< Object to deallocate.
    uint32_t status;         //!< Status regarding execution.
};

/*! \brief Resource Control Message.
 *
 * Object to send a control message to the NVIDIA kernel module.
 */
struct RmControlRes {
    uint32_t client;         //!< Client for running the object control.
    uint32_t object;         //!< Object to run the object control on.
    uint32_t cmd;            //!< Command to run on the object.
    uint32_t flags;          //!< Flags for the control of the resource.
    void* params;            //!< Parameter to pass into the control command.
    uint32_t param_size;     //!< Size of the parameter.
    uint32_t status;         //!< Status regarding execution.
};

#ifdef __cplusplus
};
#endif

#endif
