/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#include <iostream>

#include <unistd.h>
#include <cargs.h>
#include <time.h>
#include <limits.h>

#include <gpu/nvidia/resources.h>
#include <gpu/utilization.h>

#include <gvm/apis.h>
#include <gvm/legal.h>

using std::cout;

/*! \page gvm-utils GVM Utilization measurements
 * \brief Utilization daemon to provide information for the GPUs.
 *
 * \tableofcontents
 *
 * \section Introduction
 *
 * This is the daemon to run in the background and provide information
 * regarding the utilization of a GPU during the runtime of a system.
 *
 * This program for the time being only focuses on a few measures of
 * utilization, as time goes on we will have more granular control over
 * utilization.
 */

static struct cag_option options[] = {
{
.identifier = 'n',
.access_letters = "n",
.access_name = "no-print",
.value_name = NULL,
.description = "No printing."
},
{
.identifier = 'H',
.access_letters = "H",
.access_name = "hours",
.value_name = "HOURS",
.description = "Hours to measure utilization for."
},
{
.identifier = 'M',
.access_letters = "M",
.access_name = "minutes",
.value_name = "MINUTES",
.description = "Minutes to measure utilization for."
},
{
.identifier = 'S',
.access_letters = "S",
.access_name = "seconds",
.value_name = "SECONDS",
.description = "Seconds to measure utilization for."
},
{
.identifier = 'd',
.access_letters = "d",
.access_name = "daemonize",
.value_name = NULL,
.description = "Daemonizes the utilization process to run in the background."
},
{
.identifier = 'o',
.access_letters = "o",
.access_name = "output_file_base",
.value_name = NULL,
.description = "Base name of the output file."
},
{
.identifier = 'h',
.access_letters = "h",
.access_name = "help",
.value_name = NULL,
.description = "Shows the command help."
}
};

int main(int argc, char *argv[])
{
    bool daemon = false;
    bool print = true;
    char identifier;
    const char *output = NULL;
    const char *tmp = NULL;
    cag_option_context context;
    time_t end_time = INT_MAX;
    int hours = -1, minutes = -1, seconds = -1;

    // NOTE: We are only supporting NVIDIA in this release.
    enum SupportedGpus selected_gpu = Nvidia;

    cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
    while (cag_option_fetch(&context)) {
        identifier = cag_option_get(&context);
        switch (identifier) {
            case 'n':
                print = false;
                break;
            case 'H':
                tmp = cag_option_get_value(&context);
                hours = atoi(tmp);
                break;
            case 'M':
                tmp = cag_option_get_value(&context);
                minutes = atoi(tmp);
                break;
            case 'S':
                tmp = cag_option_get_value(&context);
                seconds = atoi(tmp);
                break;
            case 'd':
                daemon = true;
                break;
            case 'o':
                output = cag_option_get_value(&context);
                break;
            case 'h':
                printf(GVM_COPYRIGHT);
                printf("Usage: gvm-utils [OPTIONS]\n");
                printf("Basic tool presenting to the user the utilization that we use\n\n");
                cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
                return 0;
        }
    }

    if (daemon) {
        int child = fork();

        if (child != 0) {
            if (child == -1)
                printf("Failed to fork into daemon process.\n");
            return 0;
        }
    }

    const struct UtilizationAPI *api = UTIL_APIS[selected_gpu];
    void *api_info = NULL;
    char filename[1024] = {};
    FILE *collector = NULL;

    switch (selected_gpu) {
    case Nvidia:
        api_info = calloc(1, sizeof(struct NvUtilization));
        break;
    default:
        printf("UNSUPPORTED GPU\n");
        exit(1);
    };

    api->init(api_info);

    if (output != NULL) {
        sprintf(filename, "%s.csv", output);
        printf("Writing into filename: %s\n", filename);
        collector = fopen(filename, "w");
        fprintf(collector, "timestamp,pci_id,compute_contexts,vram_accesses,vram_alloted\n");
    }

    if (hours > 0 || minutes > 0 || seconds > 0) {
        end_time = time(NULL);
    }

    if (hours > 0) end_time += hours * 60 * 60;
    if (minutes > 0) end_time += minutes * 60;
    if (seconds > 0) end_time += seconds;

    while (time(NULL) < end_time) {
        time_t t = time(NULL);
        struct AvgGPUUtils *util = api->sample(api_info);

        if (util == NULL) {
            sleep(1);
            continue;
        }

        for (uint32_t i = 0; i < util->num_gpus; ++i) {
            struct AvgGPUUtil u = util->array[i];
            char ostr[1024] = {};

            sprintf(
                ostr,
                "%ld,%s,%f,%f,%f\n",
                t, u.bus_id, u.compute_contexts,
                u.vram_accesses, u.vram_alloted
            );

            if (collector != NULL)
                fprintf(collector, "%s", ostr);
            if (print)
                printf("%s", ostr);
        }

        sleep(1);

        free(util);
    }

    if (collector != NULL)
        fclose(collector);

    api->stop(api_info);
}
