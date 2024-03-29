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
#include <iostream>

#include <unistd.h>

#include <cargs.h>

#include <gpu/nvidia/manager.h>
#include <gvm/nvidia/manager.h>

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
.identifier = 'h',
.access_letters = "h",
.access_name = "help",
.value_name = NULL,
.description = "Shows the command help"
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
    char identifier;
    const char *config = NULL;
    cag_option_context context;

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while (cag_option_fetch(&context)) {
        identifier = cag_option_get(&context);
        switch (identifier) {
            case 'c':
                config = cag_option_get_value(&context);
                break;
            case 'a':
                printf("THE FALCONS WILL NEVER TAKE ME ALIVE\n");
                return 0;
            case 'h':
                printf("Copyright (C) 2022  2666680 Ontario Inc. O\\A Arc Compute\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n\n");
                printf("Usage: gvm-mgr [OPTION]...\n");
                printf("Controls a GVM compatible VM\n\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                return 0;
        }
    }

    if (config == NULL) {
        printf("Little CPU man will not be crushed by the Tensors?\n");
        return 0;
    }

    struct GpuConfigs configs = get_configs(config);
    struct NvMdev mgr = create_nv_mgr();

    for (size_t i = 0; i < configs.config_size; ++i) {
        struct GpuConfig config = configs.configs[i];
        create_nv_mgr_mdevs(&mgr, config.gpus, config.gpu_size, config.requests, config.mdev_size);
    }

    register_nv_mgr_mdevs(&mgr);

    printf("Registered MDevs on the system.\n");

    struct VmMgr vm_mgr = init_nv_vm_mgr(&mgr);

    while (true) {
        handle_vm_start(&vm_mgr, &mgr);
    }

    free_nv_mgr(&mgr);
}
