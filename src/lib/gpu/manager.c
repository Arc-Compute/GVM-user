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
#include <gpu/manager.h>
#include <gpu/intel/manager.h>
#include <gpu/nvidia/manager.h>

#include <utils/configs.h>

#include <stdio.h>

struct MdevMgr create_mdev_mgr(struct GpuConfigs configs)
{
    struct MdevMgr ret = {};

    if (configs.configs[0].gpu_size < 1) return ret;

    switch (configs.configs[0].gpus[0].vendor_id) {
        case 0x8086:
            ret.type = INTEL;
            ret.intel = create_intel_mgr();
            break;
        case 0x10DE:
            ret.type = NVIDIA;
            ret.nvidia = create_nv_mgr();
            break;
        default:
            ret.type = INVALID;
            printf("No mdev type created.\n");
            return ret;
    }

    for (size_t i = 0; i < configs.config_size; ++i) {
        struct GpuConfig config = configs.configs[i];

        switch (ret.type) {
            case INTEL:
                create_intel_mgr_mdevs(
                    &ret.intel, config.gpus,
                    config.gpu_size, config.requests,
                    config.mdev_size
                );
                break;
            case NVIDIA:
                 create_nv_mgr_mdevs(
                    &ret.nvidia, config.gpus,
                    config.gpu_size, config.requests,
                    config.mdev_size
                );
                break;
            default:
                printf("No mdev registration for device.\n");
                break;
        }
    }

    switch (ret.type) {
        case NVIDIA:
            register_nv_mgr_mdevs(&ret.nvidia);
            break;
        default:
            printf("No post mdev creation required.\n");
            break;
    }

    return ret;
}

void free_mdev_mgr(struct MdevMgr *mgr)
{
    switch (mgr->type) {
        case NVIDIA:
            free_nv_mgr(&mgr->nvidia);
            break;
        case INTEL:
            free_intel_mgr(&mgr->intel);
            break;
        default:
            printf("No mdev destruction required.\n");
            break;
    }
}
