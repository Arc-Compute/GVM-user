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
#include <gpu/intel/manager.h>

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*! \todo Use proper logging.
 */
struct IntelMdev create_intel_mgr()
{
    const char *dir_name = "/sys/module/i915/drivers/pci:i915";
    struct IntelMdev ret = {};

    struct dirent *de;
    DIR *dr = opendir(dir_name);

    if (dr == NULL) return ret;

    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_LNK) {
            char vendor_path[1024] = {};
            char device_path[1024] = {};
            char sub_vendor_path[1024] = {};
            char sub_device_path[1024] = {};
            char drm_path[1024] = {};
            uint32_t domain;
            uint32_t bus;
            uint32_t slot;
            uint32_t function;
            uint32_t vendor_id;
            uint32_t device_id;
            uint32_t sub_vendor_id;
            uint32_t sub_device_id;
            uint32_t card_num;

            if (sscanf(de->d_name, "%x:%x:%x.%x", &domain, &bus, &slot, &function) != 4)
                continue;

            struct IntelMdevGpu *ret = calloc(1, sizeof(struct IntelMdevGpu));

            ret->gpu.domain = domain;
            ret->gpu.bus = bus;
            ret->gpu.slot = slot;
            ret->gpu.function = function;

            sprintf(vendor_path, "%s/%s/vendor", dir_name, de->d_name);
            sprintf(device_path, "%s/%s/device", dir_name, de->d_name);
            sprintf(sub_vendor_path, "%s/%s/subsystem_vendor", dir_name, de->d_name);
            sprintf(sub_device_path, "%s/%s/subsystem_device", dir_name, de->d_name);
            sprintf(drm_path, "%s/%s/drm", dir_name, de->d_name);

            FILE *vendor_file = fopen(vendor_path, "r");
            FILE *device_file = fopen(device_path, "r");
            FILE *sub_vendor_file = fopen(sub_vendor_path, "r");
            FILE *sub_device_file = fopen(sub_device_path, "r");

            if (vendor_file) {
                fscanf(vendor_file, "0x%x", &vendor_id);
                fclose(vendor_file);
            }

            if (device_file) {
                fscanf(device_file, "0x%x", &device_id);
                fclose(device_file);
            }

            if (sub_vendor_file) {
                fscanf(sub_vendor_file, "0x%x", &sub_vendor_id);
                fclose(sub_vendor_file);
            }

            if (sub_device_file) {
                fscanf(sub_device_file, "0x%x", &sub_device_id);
                fclose(sub_device_file);
            }

            DIR *ndr = opendir(drm_path);
            struct dirent *nde;

            while ((nde = readdir(ndr)) != NULL) {
                if (sscanf(nde->d_name, "card%d", &card_num) != 1)
                    continue;
            }

            free(ndr);

            printf("Card num: %d\n", card_num);
            printf("0x%.4X 0x%.4X 0x%.4X 0x%.4X\n", vendor_id, device_id, sub_vendor_id, sub_device_id);
            printf("%s -> %.4X:%.2X:%.2X.%X\n", de->d_name, domain, bus, slot, function);

            ret->gpu.vendor_id = vendor_id;
            ret->gpu.device_id = device_id;
            ret->gpu.sub_vendor_id = sub_vendor_id;
            ret->gpu.sub_device_id = sub_device_id;
            ret->gpu.identifier = card_num;
        }
    }

    free(dr);

    return ret;
}

/*! \todo Use proper logging.
 */
void free_intel_mgr(struct IntelMdev *mgr)
{
}

void create_intel_mgr_mdevs(
    struct IntelMdev *mgr,
    struct Gpu* limited,
    size_t gpu_size,
    struct MDevRequest* requested,
    size_t mdev_size
)
{
}
