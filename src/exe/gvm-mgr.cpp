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
                printf(GVM_COPYRIGHT);
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

    printf("Not Implemented in this release.\n");
}
