/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <utils/configs.h>

#include <toml.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static inline void failed_parse(const char* s)
{
    printf("Missing required field in request config '%s', exiting program\n", s);
    exit(1);
}

struct GpuConfigs get_configs(const char* name)
{
    FILE *fp;
    char error_buffer[1024];
    struct GpuConfigs ret = {};

    fp = fopen(name, "r");
    if (fp == NULL)
        return ret;

    toml_table_t* base = toml_parse_file(fp, error_buffer, sizeof(error_buffer));
    fclose(fp);

    if (base == NULL) {
        printf("Error parsing config: %s\n", error_buffer);
        return ret;
    }

    toml_array_t* config_array = toml_array_in(base, "config");

    ret.config_size = toml_array_nelem(config_array);
    ret.configs = calloc(ret.config_size, sizeof(struct GpuConfig));

    for (size_t i = 0; i < ret.config_size; ++i) {
        toml_table_t* config = toml_table_at(config_array, i);

        toml_array_t* gpu_array = toml_array_in(config, "gpu_config");
        ret.configs[i].gpu_size = toml_array_nelem(gpu_array);
        size_t max_gpu_size = ret.configs[i].gpu_size * sizeof(struct Gpu);
        ret.configs[i].gpus = malloc(max_gpu_size);
        memset(ret.configs[i].gpus, 0xFF, max_gpu_size);

        for (size_t j = 0; j < ret.configs[i].gpu_size; ++j) {
            toml_table_t* gpu = toml_table_at(gpu_array, j);
            toml_datum_t info = {};

            info = toml_int_in(gpu, "domain");
            if (info.ok)
                ret.configs[i].gpus[j].domain = info.u.i;

            info = toml_int_in(gpu, "bus");
            if (info.ok)
                ret.configs[i].gpus[j].bus = info.u.i;

            info = toml_int_in(gpu, "slot");
            if (info.ok)
                ret.configs[i].gpus[j].slot = info.u.i;

            info = toml_int_in(gpu, "function");
            if (info.ok)
                ret.configs[i].gpus[j].function = info.u.i;

            info = toml_int_in(gpu, "vendor_id");
            if (info.ok)
                ret.configs[i].gpus[j].vendor_id = info.u.i;

            info = toml_int_in(gpu, "device_id");
            if (info.ok)
                ret.configs[i].gpus[j].device_id = info.u.i;

            info = toml_int_in(gpu, "sub_vendor_id");
            if (info.ok)
                ret.configs[i].gpus[j].sub_vendor_id = info.u.i;

            info = toml_int_in(gpu, "sub_device_id");
            if (info.ok)
                ret.configs[i].gpus[j].sub_device_id = info.u.i;

            info = toml_int_in(gpu, "identifier");
            if (info.ok)
                ret.configs[i].gpus[j].identifier = info.u.i;

            info = toml_int_in(gpu, "number");
            if (info.ok)
                ret.configs[i].gpus[j].number = info.u.i;

            info = toml_int_in(gpu, "free_fb");
            if (info.ok)
                ret.configs[i].gpus[j].free_fb = info.u.i;

            info = toml_int_in(gpu, "total_fb");
            if (info.ok)
                ret.configs[i].gpus[j].total_fb = info.u.i;
        }

        toml_array_t* req_array = toml_array_in(config, "request");
        ret.configs[i].mdev_size = toml_array_nelem(req_array);
        ret.configs[i].requests = calloc(ret.configs[i].mdev_size, sizeof(struct MDevRequest));

        for (size_t j = 0; j < ret.configs[i].mdev_size; ++j) {
            toml_table_t* mdev = toml_table_at(req_array, j);
            toml_table_t* display = toml_table_in(mdev, "display");
            toml_datum_t info = {};

            info = toml_int_in(mdev, "num");
            if (!info.ok)
                failed_parse("num");

            ret.configs[i].requests[j].num = info.u.i;

            info = toml_int_in(mdev, "fb_len");
            if (!info.ok)
                failed_parse("fb_len");

            ret.configs[i].requests[j].fb_len = info.u.i;

            info = toml_int_in(mdev, "fb_res");
            if (!info.ok)
                failed_parse("fb_res");

            ret.configs[i].requests[j].fb_res = info.u.i;

            info = toml_int_in(mdev, "max_instances");
            if (!info.ok)
                failed_parse("max_instances");

            ret.configs[i].requests[j].max_inst = info.u.i;

            info = toml_int_in(mdev, "v_dev_id");
            ret.configs[i].requests[j].v_dev_id =
                info.ok ? ((unsigned) info.u.i) : 0xFFFFFFFFFFFFFFFF;

            info = toml_int_in(mdev, "p_dev_id");
            ret.configs[i].requests[j].p_dev_id =
                info.ok ? ((unsigned) info.u.i) : 0xFFFFFFFFFFFFFFFF;

            info = toml_bool_in(mdev, "ecc_support");
            ret.configs[i].requests[j].ecc_support =
                info.ok ? info.u.b : 0;

            info = toml_bool_in(mdev, "multi_mdev");
            ret.configs[i].requests[j].multi_mdev =
                info.ok ? info.u.b : 0;

            info = toml_int_in(mdev, "map_video_size");
            ret.configs[i].requests[j].map_vid_size =
                info.ok ? info.u.i : 24;

            info = toml_int_in(mdev, "enc_cap");
            ret.configs[i].requests[j].enc_cap =
                info.ok ? info.u.i : 100;

            info = toml_int_in(mdev, "bar1_len");
            ret.configs[i].requests[j].bar1_len =
                info.ok ? info.u.i : 0x100;

            info = toml_string_in(mdev, "name");
            ret.configs[i].requests[j].name =
                info.ok ? info.u.s : "GVM GPU";

            info = toml_string_in(mdev, "class");
            ret.configs[i].requests[j].gpu_class =
                info.ok ? info.u.s : "Compute";

            ret.configs[i].requests[j].disp = calloc(1, sizeof(struct VirtDisplay));

            info.ok = 0;
            if (display)
                info = toml_int_in(display, "num_heads");
            ret.configs[i].requests[j].disp->num_heads =
                info.ok ? info.u.i : 1;

            info.ok = 0;
            if (display)
                info = toml_int_in(display, "max_res_x");
            ret.configs[i].requests[j].disp->max_res_x =
                info.ok ? info.u.i : 1024;

            info.ok = 0;
            if (display)
                info = toml_int_in(display, "max_res_y");
            ret.configs[i].requests[j].disp->max_res_y =
                info.ok ? info.u.i : 1024;

            info.ok = 0;
            if (display)
                info = toml_int_in(display, "frl_config");
            ret.configs[i].requests[j].disp->frl_config =
                info.ok ? info.u.i : 120;

            info.ok = 0;
            if (display)
                info = toml_int_in(display, "frl_enable");
            ret.configs[i].requests[j].disp->frl_enable =
                info.ok ? info.u.i : 1;
        }
    }

    toml_free(base);

    return ret;
}
