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
#include <utils/device.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*! \failure Invalid File - The only way this function can fail is by not having
 *                          the file in question exist.
 */
uint8_t device_exists(const char* device_name)
{
    char new_device[1024] = "/dev/";
    uint8_t ret = 0;
    strcat(new_device, device_name);
    ret = (access(device_name, F_OK) == 0) || (access(new_device, F_OK) == 0);
    return ret;
}

/*! \failure Invalid Device - In the case of an invalid device being passed it will return -1.
 * \failure Device List Not Found - Device list is not found, this can occur on unsupported operating systems.
 */
int32_t get_major(const char* device)
{
    int32_t major = -1;
    FILE *in_file = fopen("/proc/devices", "r");

    struct stat sb;
    stat("/proc/devices", &sb);

    char file_contents[1024] = "";

    while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
        char name[1024] = "";

        int correct_format = sscanf(file_contents, "%d %s", &major, name);
        int compare = strcmp(name, device);

        if (correct_format == 2 && compare == 0)
            break;

        major = -1;
    }

    fclose(in_file);

    return major;
}

/*! \failure Param File Not Found - Check if the device driver is loaded correctly.
 */
int32_t get_param(const char* filename, const char* parameter)
{
    int32_t ret = -1;
    FILE *in_file = fopen(filename, "r");

    struct stat sb;
    stat(filename, &sb);

    char file_contents[1024] = "";
    char name[1024] = "";
    sprintf(name, "%s: %%d", parameter);

    while (fscanf(in_file, "%[^\n] ", file_contents) != EOF) {
        int correct_format = sscanf(file_contents, name, &ret);

        if (correct_format == 1)
            break;

        ret = -1;
    }

    fclose(in_file);

    return ret;
}
