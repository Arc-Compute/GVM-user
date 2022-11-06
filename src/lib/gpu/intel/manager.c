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
#include <fcntl.h>

#define STOP "stop"
#define PAUSE "pause"
#define RESUME "resume"
#define CLEAR "clear"

static void set_numvfs(const int id, const int numvfs)
{
    const char *device_folder = "/sys/class/drm/card%d/device";
    char drm_card[80];
    char autoprobe_name[128];
    char numvfs_name[128];

    sprintf(drm_card, device_folder, id);
    sprintf(autoprobe_name, "%s/sriov_drivers_autoprobe", drm_card);
    sprintf(numvfs_name, "%s/sriov_numvfs", drm_card);

    FILE *autoprobe_file = fopen(autoprobe_name, "w");
    FILE *numvfs_file = fopen(numvfs_name, "w");

    printf("Updating autoprobe to false for card%d\n", id);

    if (autoprobe_file) {
        fprintf(autoprobe_file, "%d", 0);
    }

    printf("Setting numvfs for card%d to %d\n", id, numvfs);

    if (numvfs_file) {
        fprintf(numvfs_file, "%d", numvfs);
        fclose(numvfs_file);
    }

    printf("Updating autoprobe to true for card%d\n", id);

    if (autoprobe_file) {
        fprintf(autoprobe_file, "%d", 1);
        fclose(autoprobe_file);
    }
}

static void enable_vfio_pci(const int vendor, const int device)
{
    const char *vfio_pci_name = "/sys/bus/pci/drivers/vfio-pci/new_id";
    FILE *vfio_pci_file = fopen(vfio_pci_name, "w");

    if (vfio_pci_file) {
        fprintf(vfio_pci_file, "%.4x %.4x", vendor, device);
        fclose(vfio_pci_file);
    }
}

static void clear_gpu(struct IntelMdevGpu* gpu)
{
    for (size_t j = 0; j < gpu->partitions && gpu->vfs[j] != NULL; ++j) {
        struct IntelVF *vf = gpu->vfs[j];

        printf("Clearing VF: %d\n", vf->fn);

        if (vf->active) {
            close(vf->control_fd);
        }

        free(vf);
        gpu->vfs[j] = NULL;
    }

    set_numvfs(gpu->gpu.identifier, 0);
    gpu->partitions = 0;
    gpu->used_fb = 0;
    gpu->num = 0;
}

/*! \todo Use proper logging.
 */
struct IntelMdev create_intel_mgr()
{
    const char *dir_name = "/sys/module/i915/drivers/pci:i915";
    struct IntelMdev gpu_list = {};
    uint32_t i = 0;

    struct dirent *de;
    DIR *dr = opendir(dir_name);

    if (dr == NULL) return gpu_list;

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

            uint8_t n = 1;

            FILE *vendor_file = fopen(vendor_path, "r");
            FILE *device_file = fopen(device_path, "r");
            FILE *sub_vendor_file = fopen(sub_vendor_path, "r");
            FILE *sub_device_file = fopen(sub_device_path, "r");

            if (vendor_file) {
                n = n && fscanf(vendor_file, "0x%x", &vendor_id) != -1;
                fclose(vendor_file);
            }

            if (device_file) {
                n = n && fscanf(device_file, "0x%x", &device_id) != -1;
                fclose(device_file);
            }

            if (sub_vendor_file) {
                n = n && fscanf(sub_vendor_file, "0x%x", &sub_vendor_id) != -1;
                fclose(sub_vendor_file);
            }

            if (sub_device_file) {
                n = n && fscanf(sub_device_file, "0x%x", &sub_device_id) != -1;
                fclose(sub_device_file);
            }

            if (!n) printf("Could not read necessary files\n");

            DIR *ndr = opendir(drm_path);
            struct dirent *nde;

            while ((nde = readdir(ndr)) != NULL) {
                if (sscanf(nde->d_name, "card%d", &card_num) != 1)
                    continue;
            }

            free(ndr);

            ret->gpu.vendor_id = vendor_id;
            ret->gpu.device_id = device_id;
            ret->gpu.sub_vendor_id = sub_vendor_id;
            ret->gpu.sub_device_id = sub_device_id;
            ret->gpu.identifier = card_num;

            ret->num = 0;

            switch (device_id) {
                case 0x56C0:
                    ret->max_fb = 16384;
                    break;
                case 0x56C1:
                    ret->max_fb = 12288;
                    break;
                default:
                    printf("Not currently supported");
                    free(ret);
                    ret = NULL;
                    break;
            }

            if (ret == NULL) continue;

            ret->remaining = 0;

            clear_gpu(ret);

            gpu_list.gpus[i++] = ret;
        }
    }

    free(dr);

    return gpu_list;
}

/*! \todo Use proper logging.
 */
void free_intel_mgr(struct IntelMdev *mgr)
{
    for (int i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct IntelMdevGpu *gpu = mgr->gpus[i];

        clear_gpu(gpu);

        free(gpu);

        mgr->gpus[i] = NULL;
    }
}

void create_intel_mgr_mdevs(
    struct IntelMdev *mgr,
    struct Gpu* limited,
    size_t gpu_size,
    struct MDevRequest* requested,
    size_t mdev_size
)
{
    for (size_t i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct IntelMdevGpu *gpu = mgr->gpus[i];
        struct Gpu *ggpu = &(gpu->gpu);
        uint8_t valid = 0;

        for (size_t j = 0; j < gpu_size && !valid; ++j) {
            struct Gpu req_gpu = limited[j];

            valid =
                (req_gpu.domain == 0xFFFFFFFF ||
                 req_gpu.domain == ggpu->domain) &&
                (req_gpu.bus == 0xFFFFFFFF ||
                 req_gpu.bus == ggpu->bus) &&
                (req_gpu.slot == 0xFFFFFFFF ||
                 req_gpu.slot == ggpu->slot) &&
                (req_gpu.function == 0xFFFFFFFF ||
                 req_gpu.function == ggpu->function) &&
                (req_gpu.vendor_id == 0xFFFFFFFF ||
                 req_gpu.vendor_id == ggpu->vendor_id) &&
                (req_gpu.device_id == 0xFFFFFFFF ||
                 req_gpu.device_id == ggpu->device_id) &&
                (req_gpu.sub_vendor_id == 0xFFFFFFFF ||
                 req_gpu.sub_vendor_id == ggpu->sub_vendor_id) &&
                (req_gpu.sub_device_id == 0xFFFFFFFF ||
                 req_gpu.sub_device_id == ggpu->sub_device_id) &&
                (req_gpu.identifier == 0xFFFFFFFF ||
                 req_gpu.identifier == ggpu->identifier);
        }

        if (!valid)
            continue;

        for (size_t j = 0; j < 128; ++j)
            gpu->available[j] = NULL;

        for (size_t j = 0; j < 128 && j < mdev_size; ++j)
            gpu->available[j] = &requested[j];
    }
}

struct IntelVF* find_vf(struct IntelMdev *mgr, uint32_t num)
{
    struct IntelVF *vf = NULL;

    printf("Finding a VF for Intel GPU\n");

    for (size_t i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct IntelMdevGpu *gpu = mgr->gpus[i];

        for (size_t j = 0; j < 128 && gpu->available[j] != NULL && vf == NULL; ++j) {
            char control_path[128] = {};
            struct MDevRequest *mdev = gpu->available[j];
            if (mdev->num != num)
                continue;

            if ((gpu->partitions != 0 || gpu->num != 0) && gpu->num != num)
                break;

            if (gpu->partitions == 0) {
                set_numvfs(gpu->gpu.identifier, mdev->max_inst);
                enable_vfio_pci(gpu->gpu.vendor_id, gpu->gpu.device_id);
                gpu->remaining = mdev->max_inst;
            }

            gpu->partitions = mdev->max_inst;
            gpu->num = num;

            for (size_t k = 0; k < gpu->partitions; ++k) {
                if (gpu->vfs[k] == NULL) {
                    gpu->vfs[k] = calloc(1, sizeof(struct IntelVF));
                    gpu->vfs[k]->fn = k + 1;
                    gpu->vfs[k]->fb = mdev->fb_len;
                    gpu->vfs[k]->active = 0;
                }

                if (vf == NULL && gpu->vfs[k]->active == 0) {
                    vf = gpu->vfs[k];
                    vf->active = 1;
                    sprintf(
                        control_path,
                        "/sys/class/drm/card%d/iov/vf%d/control",
                        gpu->gpu.identifier,
                        vf->fn
                    );
                    vf->control_fd = open(control_path, O_RDWR);

                    printf(
                        "Found VF on gpu %.4X:%.2X:%.2X.%X\n",
                        gpu->gpu.domain,
                        gpu->gpu.bus,
                        gpu->gpu.slot,
                        vf->fn
                    );
                    gpu->remaining -= 1;
                    gpu->used_fb += vf->fb;
                }
            }
        }
    }

    return vf;
}

void remove_vf(struct IntelMdev *mgr, struct Gpu *vgpu)
{
    printf(
        "Deleting VF on gpu %.4X:%.2X:%.2X.%X\n",
        vgpu->domain,
        vgpu->bus,
        vgpu->slot,
        vgpu->function
    );
    for (size_t i = 0; i < 32 && mgr->gpus[i] != NULL; ++i) {
        struct IntelMdevGpu *gpu = mgr->gpus[i];

        if (gpu->gpu.domain == vgpu->domain &&
            gpu->gpu.bus == vgpu->bus &&
            gpu->gpu.slot == vgpu->slot) {
            struct IntelVF *vf = gpu->vfs[vgpu->function - 1];

            if (vf->fn != vgpu->function || !vf->active) {
                printf("Incorrect VF.\n");
                continue;
            }

            vf->active = 0;
            close(vf->control_fd);

            gpu->used_fb -= vf->fb;

            gpu->remaining += 1;

            if (gpu->remaining == gpu->partitions && gpu->used_fb == 0) {
                clear_gpu(gpu);
            }
        }
    }
}
