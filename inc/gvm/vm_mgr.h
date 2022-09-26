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
#ifndef GVM_VM_MGR_H
#define GVM_VM_MGR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Structure for VM management.
 *
 * This structure is used as an intermediate object
 * for getting information from a VM when opened.
 */
struct VmMgr {
    int event_start;            //!< Event start fd.
    int event_bind;             //!< Event bind fd.
    uint32_t root;              //!< Root of the VM manager.
    int mdev_fd;                //!< MDEV file descriptor.
};

/*! \brief File descriptors for VM control.
 *
 * This is a structure which acts as a general purpose wrapper around
 * all different components of VM control.
 */
struct VmListener {
    int stop_fd;                //!< File descriptor to stop the VM.
    int reg_fd;                 //!< File descriptor to access registers.
    int reset_fd;               //!< File descriptor to reboot the VM.
    int console_fd;             //!< File descriptor to access console.
};

#ifdef __cplusplus
};
#endif

#endif
