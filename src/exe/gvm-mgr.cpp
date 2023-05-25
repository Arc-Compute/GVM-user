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
#include <gvm/nvidia/manager.h>
#include <gvm/nvidia/open/signature.h>

#include <utils/configs.h>

using std::cout;

/*! \page gvm-mgr GVM Manager
 * \brief Controls the attachment of mediated devices to VMs.
 *
 * \tableofcontents
 *
 * \section Introduction
 *
 * This code creates and attaches mediated devices to the VMs running on
 * the host device.
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
.identifier = 'p',
.access_letters = "p",
.access_name = "plugin",
.value_name = "PLUGIN",
.description = "Plugin (VMIOP) to use."
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
    const char *plugin = NULL;
    cag_option_context context;

    uint8_t strict_api = 0;
    uint32_t errors = 0;

    // NOTE: We are only supporting NVIDIA in this release.
    enum SupportedGpus selected_gpu = Nvidia;

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while (cag_option_fetch(&context)) {
        identifier = cag_option_get(&context);
        switch (identifier) {
            case 'c':
                config = cag_option_get_value(&context);
                break;
            case 'p':
                plugin = cag_option_get_value(&context);
                break;
            case 's':
                strict_api = 1;
                break;
            case 'a':
                printf("THE FALCONS WILL NEVER TAKE ME ALIVE\n");
                return 0;
            case 'h':
                printf(GVM_COPYRIGHT);
                printf("Usage: gvm-mgr [OPTION]...\n");
                printf("Controls a GVM compatible VM\n\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                return 0;
        }
    }

    const struct ManagerAPI *api = MANAGER_APIS[selected_gpu];
    void *api_info = NULL;

    switch (selected_gpu) {
    case Nvidia: {
        struct NvManager *a = NULL;
        api_info = calloc(1, sizeof(struct NvManager));
        a = (struct NvManager*) api_info;
        a->ignore_version_check = !strict_api;
        a->create_signature = nvidia_open_create_signature;
        a->use_vmiop = 1;
        a->vmiop_plugin =
            plugin == NULL ?
            "/usr/lib/x86_64-linux-gnu/libnvidia-vgpu.so" : plugin;
        break;
    }
    default:
        printf("UNSUPPORTED GPU\n");
        exit(1);
    };

    if (api->init(api_info)) {
        printf("Initializing the manager API failed.\n");
        ret = 1;
        goto finished;
    }

    if (config != NULL) {
        struct GpuConfigs configs = get_configs(config);

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
    }

    while (api->running(api_info)) {
        errors += api->handle_event(api_info);

        if (errors > 10)
            break;
    }

    api->stop(api_info);

finished:
    free(api_info);
    return ret;
}
