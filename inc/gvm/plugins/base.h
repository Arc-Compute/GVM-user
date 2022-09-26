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
#ifndef GVM_PLUGINS_BASE_H
#define GVM_PLUGINS_BASE_H

#include <gvm/vm_mgr.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Sets up the plugin environment for nvidia.
 *
 * This code sets up the plugin environment to be used by the plugin system.
 *
 * \params instance_id - Id of the MDEV device.
 * \return Returns a listener for the VM object.
 */
struct VmListener nv_plugin_env_setup(uint16_t instance_id);

/*! \brief Listens for commands on the plugin.
 *
 * This code runs the plugin's codebase.
 *
 * \params vm_ctrl - Controller for the VM.
 * \return Runs the plugin.
 */
void nv_plugin_start(struct VmListener *vm_ctrl);

#ifdef __cplusplus
};
#endif

#endif
