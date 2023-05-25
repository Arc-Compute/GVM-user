/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <iostream>

#include <unistd.h>

#include <cargs.h>

#include <gvm/apis.h>
#include <gvm/legal.h>

#include <gvm/nvidia/creator.h>

#include <gvm/nvidia/open/signature.h>

#include <utils/configs.h>

using std::cout;

/*! \page gvm-cli GVM CLI
 * \brief Controls the creation of arbitrary mediated devices on the system.
 *
 * \tableofcontents
 *
 * \section Introduction
 *
 * This code creates a list of arbitrary mediated devices in the kernel
 * module, which can be created and used by LibVF.IO based programs. For
 * now we only allow the creation of NVIDIA mediated devices, future plans
 * are in the works to provide us the ability to create mediated devices
 * on multiple GPU vendors.
 */

static struct cag_option options[] = {
{
.identifier = 'c',
.access_letters = "c",
.access_name = "config",
.value_name = "CONFIG",
.description = "Configuration file to use."
},
{
.identifier = 'h',
.access_letters = "h",
.access_name = "help",
.value_name = NULL,
.description = "Shows the command help"
},
{
.identifier = 's',
.access_letters = "s",
.access_name = "strict-api",
.value_name = NULL,
.description = "Uses a strict API which must be matched to run code"
},
{
.identifier = 'a',
.access_letters = "a",
.access_name = "advanced-help",
.value_name = NULL,
.description = "Shows the advanced help"
}
};

int main(int argc, char *argv[])
{
    int ret = 0;
    char identifier;
    const char *config = NULL;
    cag_option_context context;

    uint8_t strict_api = 0;

    // NOTE: We are only supporting NVIDIA in this release.
    enum SupportedGpus selected_gpu = Nvidia;

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while (cag_option_fetch(&context)) {
        identifier = cag_option_get(&context);
        switch (identifier) {
            case 'c':
                config = cag_option_get_value(&context);
                break;
            case 's':
                strict_api = 1;
                break;
            case 'a':
                printf("HELP THE FALCONS ARE CHASING ME...\n");
                return 0;
            case 'h':
                printf(GVM_COPYRIGHT);
                printf("Usage: gvm-cli [OPTION]...\n");
                printf("Reimplementation of mdev-gpu under the GVM User Suite\n\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                return 0;
        }
    }

    if (config == NULL) {
        printf("Why must the Tensor Cores crush the little cpu man?\n");
        return 0;
    }

    const struct CreatorAPI *api = CREATOR_APIS[selected_gpu];
    void *api_info = NULL;

    switch (selected_gpu) {
    case Nvidia: {
        struct NvCreator *a = NULL;
        api_info = calloc(1, sizeof(struct NvCreator));
        a = (struct NvCreator*) api_info;
        a->ignore_version_check = !strict_api;
        a->create_signature = nvidia_open_create_signature;
        break;
    }
    default:
        printf("UNSUPPORTED GPU\n");
        exit(1);
    };

    struct GpuConfigs configs = get_configs(config);

    if (api->init(api_info)) {
        printf("Initializing the creator API failed.\n");
        ret = 1;
        goto finished;
    }

    for (size_t i = 0; i < configs.config_size; ++i) {
        struct GpuConfig config = configs.configs[i];

        printf(
            "Gpu size: %ld\nRequests size: %ld\n",
            config.gpu_size, config.mdev_size
        );

        api->create_mdevs(
            api_info,
            config.requests, config.mdev_size,
            config.gpus, config.gpu_size
        );
    }

    api->register_mdevs(api_info);

    api->stop(api_info);

finished:
    free(api_info);
    return ret;
}
