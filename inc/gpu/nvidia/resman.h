/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GPU_NVIDIA_RESMAN_H
#define GPU_NVIDIA_RESMAN_H

#include <stdio.h>
#include <stdint.h>

#include <gpu/nvidia/resources.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Version check for the RM API.
 *
 * RM API version check must be performed before running code.
 *
 * \sideeffect RM Side Effect: Enables the file descriptor to connect to the device.
 *
 * \param ctl_fd - The RM version check can only be performed on the ctl file.
 * \param ignore_version - If we ignore the version of the RM API.
 * \param version - Version we use for the RMCore.
 * \return If we were successfully version checked or not.
 */
uint8_t rm_version_check(int ctl_fd, uint8_t ignore_version, const char* version);

/*! \brief Allocates a Node for a resource.
 *
 * The RM Core uses an object oriented paradigm which forces us to create nodes and pass them into new regions.
 *
 * \sideeffect RM Side Effect: Allocates an object with a specific class in the RM Core, for safety purposes
 *                             we should call a free res for all resources available on the system. However it is
 *                             not strictly necessary as there is a garbage collector inside the RM Core.
 * \sideeffect State Side Effect: Parent node is modified, to have a new child.
 *
 * \param fd - File for allocating the device on.
 * \param parent - Parent node to allocate the device on.
 * \param object - Object id.
 * \param rm_class - Class of the new object.
 * \param data - Pointer for allocation data.
 * \return New object resource, or NULL in a case of a failure.
 */
struct NvResource* rm_alloc_res(
    int fd,
    struct NvResource* parent,
    uint32_t object,
    uint32_t rm_class,
    void* data
);

/*! \brief Frees a Node for a Resource.
 *
 * The RM Core uses an object oriented paradigm which forces us to free nodes when we are finished. This specifically
 * is a deconstructor call for an arbitrary resource.
 *
 * \sideeffect RM Side Effect: Deallocates an object with a specific id in the RM Core.
 *
 * \param fd - File for deallocating the device on.
 * \param object - Object we are deallocating.
 * \return Boolean value if we could free or not free the object in question.
 */
uint8_t rm_free_res(int fd, struct NvResource* object);

/*! \brief Frees a resource tree for the system.
 *
 * This is a mechanism by which we can free an entire tree.
 *
 * \sideeffect RM Side Effect: Deallocates an object with a specific root in the RM Core.
 *
 * \param fd - File for deallocating the device on.
 * \param root - Root we are deallocating from.
 */
void rm_free_tree(int fd, struct NvResource* root);

/*! \brief Allocates an Operating System Event.
 *
 * This is how we can create an operating system event.
 *
 * \param fd - File descriptor to allocate on the OS.
 * \param client_id - Client ID.
 * \param device_id - Device ID.
 * \return Allocated an operating system event.
 */
uint8_t rm_alloc_os_event(int fd, uint32_t client_id, uint32_t device_id);

/*! \brief Control resource command.
 *
 * The RM Core uses an object oriented paradigm which forces us to use this generic function to apply methods onto
 * the node in question. This specifically is a method application call for an arbitrary resource.
 *
 * \sideeffect RM Side Effect: Performs a potentially unsafe method on an object in the kernel.
 * \sideeffect Log Side Effect: Logs incorrect information.
 *
 * \param fd - File for controlling object on.
 * \param client - Client id to use.
 * \param device - Object id to use.
 * \param command - Command to use on the object.
 * \param data - Command data.
 * \param size - Size of the data.
 * \return Pointer to the command data, NULL in terms of failure.
 */
void* rm_ctrl_res(int fd, uint32_t client, uint32_t device, uint32_t command, void* data, uint32_t size);

/*! \brief Controls a RM Resource.
 *
 * This is a macro to simplify code for controlling resources.
 *
 * \sideeffect Macro for rm_ctrl_res
 *
 * \param fd - File for controlling object on.
 * \param res - Resource pointer.
 * \param cmd - Command to run.
 * \param data - Data to use.
 * \return Pointer to the command data, NULL in terms of failure.
 */
#define RM_CTRL(fd, res, cmd, data) rm_ctrl_res(fd, res->client, res->object, cmd, &data, sizeof(data))

/*! \brief Controls a RM Resource.
 *
 * This is a macro to simplify code for controlling resources.
 *
 * \sideeffect Macro for rm_ctrl_res
 *
 * \param fd - File for controlling object on.
 * \param client - Client id.
 * \param object - Object id.
 * \param cmd - Command to run.
 * \param data - Data to use.
 * \return Pointer to the command data, NULL in terms of failure.
 */
#define _RM_CTRL(fd, client, object, cmd, data) rm_ctrl_res(fd, client, object, cmd, &data, sizeof(data))

#ifdef __cplusplus
};
#endif

#endif
