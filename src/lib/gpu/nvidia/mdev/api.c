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
#include <gpu/nvidia/mdev/api.h>
#include <gpu/nvidia/mdev/ioctl.h>
#include <gpu/nvidia/mdev/types.h>

#include <gpu/nvidia/device.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*! \todo Proper logging if a failure occurs.
 */
int nv_mdev_create_event(uint32_t event_type, uint16_t id)
{
    struct NvMdevIoctl mdev_event = {};

    int fd = nv_open_mdev(id);

    mdev_event.params = &event_type;
    mdev_event.size = sizeof(event_type);

    if (ioctl(fd, NV_MDEV_CREATE_EVENT, &mdev_event) == -1 ||
        mdev_event.status != 0) {
        printf("Error creating mdev event: 0x%.8X\n", mdev_event.status);
        close(fd);
        return -1;
    }

    return fd;
}
