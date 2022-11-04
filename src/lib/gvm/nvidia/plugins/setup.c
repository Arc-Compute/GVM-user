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
#include <gvm/plugins/base.h>

#include <gpu/nvidia/mdev/api.h>

#include <sys/select.h>

#include <stdlib.h>

/*! \todo Proper logging.
 */
struct VmListener nv_plugin_env_setup(uint16_t instance_id)
{
    struct VmListener ret = {};

    ret.stop_fd = nv_mdev_create_event(2, instance_id);
    ret.reg_fd = nv_mdev_create_event(1, instance_id);
    ret.reset_fd = nv_mdev_create_event(3, instance_id);
    ret.console_fd = nv_mdev_create_event(5, instance_id);

    return ret;
}

/*! \todo Proper logging.
 */
void nv_plugin_start(struct VmListener *vm_ctrl)
{
    fd_set     read_fds;

    int max_fd = vm_ctrl->stop_fd;

    max_fd = max_fd > vm_ctrl->reg_fd ? max_fd : vm_ctrl->reg_fd;
    max_fd = max_fd > vm_ctrl->reset_fd ? max_fd : vm_ctrl->reset_fd;
    max_fd = max_fd > vm_ctrl->console_fd ? max_fd : vm_ctrl->console_fd;

    ++max_fd;

    while (1) {
        FD_ZERO(&read_fds);

        FD_SET(vm_ctrl->stop_fd, &read_fds);
        FD_SET(vm_ctrl->reg_fd, &read_fds);
        FD_SET(vm_ctrl->reset_fd, &read_fds);
        FD_SET(vm_ctrl->console_fd, &read_fds);

        select(max_fd, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(vm_ctrl->stop_fd, &read_fds)) {
            printf("Attempting to shutdown the VM.\n");
            exit(0);
        } else if (FD_ISSET(vm_ctrl->reg_fd, &read_fds)) {
            printf("Attempting to access a hardware register in the mdev.\n");
        } else if (FD_ISSET(vm_ctrl->reset_fd, &read_fds)) {
            printf("Rebooting the VM.\n");
        } else if (FD_ISSET(vm_ctrl->console_fd, &read_fds)) {
            printf("Accessing the console.\n");
        }
    }
}
