/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <iostream>

#include <unistd.h>

#include <cargs.h>

#include <gvm/legal.h>

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
}
