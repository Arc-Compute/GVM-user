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
                printf("HELP THE FALCONS ARE CHASING ME...\n");
                return 0;
            case 'h':
                printf("Copyright (C) 2022  2666680 Ontario Inc. O\\A Arc Compute\n\nThis program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n\n");
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

    struct GpuConfigs configs = get_configs(config);
    struct MdevMgr mgr = create_mdev_mgr(configs);

    printf("Registered configs\n");

    free_mdev_mgr(&mgr);
}
