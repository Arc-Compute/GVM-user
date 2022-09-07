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
#include <gpu/nvidia/device.h>
#include <utils/device.h>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define MAKE_DEV(major, minor)                                      \
    ((dev_t) ((minor & 0xff) | (major << 8) | ((minor & ~0xff) << 12)))

/*! \failure Not Created - Normally this is the case if the NVIDIA driver is not loaded.
 */
int nv_open(
    int32_t major,
    uint16_t minor,
    const char* path,
    const char* proc_path
)
{
    if (major == -1)
        return -1;

    int fd = -1;
    dev_t dev = MAKE_DEV(major, minor);
    uid_t uid = get_param(proc_path, "DeviceFileUID");
    gid_t gid = get_param(proc_path, "DeviceFileGID");
    mode_t mode = get_param(proc_path, "DeviceFileMode");

    if (!device_exists(path)) {
        if (mknod(path, mode | S_IFCHR, dev) == -1) {
            return fd;
        }
    }

    if (chown(path, uid, gid) == -1)
        return fd;

    fd = open(path, O_RDWR | O_CLOEXEC);

    return fd;
}

/*! \failure Not Created - Normally this is the case if the NVIDIA driver is not loaded.
 */
int nv_open_dev(uint16_t minor)
{
    const char *params = "/proc/driver/nvidia/params";

    int32_t major = get_major("nvidia-frontend");
    char path[1024] = "";

    if (minor == 255)
        sprintf(path, "/dev/nvidiactl");
    else
        sprintf(path, "/dev/nvidia%d", minor);

    return nv_open(major, minor, path, params);
}

/*! \failure Not Created - Normally this is the case if the NVIDIA driver is not loaded.
 */
int nv_open_mdev(uint16_t minor)
{
    const char *params = "/proc/driver/nvidia/params";

    int32_t major = get_major("nvidia-vgpu-vfio");
    char path[1024] = "";

    sprintf(path, "/dev/nvidia-vgpu%d", minor);

    return nv_open(major, minor, path, params);
}
