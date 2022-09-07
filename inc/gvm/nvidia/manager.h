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
#ifndef GVM_NVIDIA_MANAGER_H
#define GVM_NVIDIA_MANAGER_H

#include <gpu/nvidia/resources.h>
#include <gvm/vm_mgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Initializes a NVIDIA VM manager to connect to a VM.
 *
 * This code initializes a NVIDIA VM manager object that will
 * help us in providing a VM manager command.
 *
 * \sideeffect RM Side Effect: Creates a start and a bind operation to the VM object.
 *
 * \param mdevs - Root for all mdevs.
 * \return A VM manager object.
 */
struct VmMgr init_nv_vm_mgr(struct NvMdev* mdevs);

/*! \brief Handles a start request.
 *
 * Handles a VM start request.
 *
 * \sideeffect VM Side Effect: Starts a VM.
 *
 * \param mgr - Manager for the VM management system.
 * \param mdev_mgr - Manager for the mediated devices.
 * \return Only performed for the side effects.
 */
void handle_vm_start(struct VmMgr* mgr, struct NvMdev* mdev_mgr);

/*! \brief Connects the VM to the user program.
 *
 * Tells the kernel to start the VM.
 *
 * \sideeffect VM Side Effect: Starts a VM.
 *
 * \param mgr - Manager for the VM management system.
 * \param mdev_mgr - Manager for the mediated devices.
 * \return Only performed for the side effects.
 */
void start_vm(struct VmMgr* mgr, struct NvMdev* mdev_mgr);

#ifdef __cplusplus
};
#endif

#endif
