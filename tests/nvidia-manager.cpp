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

#include <gpu/nvidia/manager.h>
#include <gpu/nvidia/resources.h>

#include <utils/colors.h>

using std::cout;

/*! \page nvidia-mgr-test NVIDIA Manager Test
 *
 * \tableofcontents
 *
 * \todo Create more tests + documentation for nvidia manager.
 *
 */

bool create_mdevs()
{
    struct NvMdev mgr = create_nv_mgr();
    struct VirtDisplay display = {};
    struct MDevRequest requested = {};

    display.num_heads = 4;
    display.max_res_x = 1024;
    display.max_res_y = 1024;
    display.frl_config = 120;
    display.frl_enable = 0;

    requested.disp = &display;
    requested.num = 1;
    requested.v_dev_id = 0x1B301B30;
    requested.p_dev_id = 0x1B30;
    requested.name = "GVM";
    requested.gpu_class = "GeForce";
    requested.max_inst = 1;
    requested.fb_len = 896;
    requested.fb_res = 128;
    requested.map_vid_size = 4;
    requested.enc_cap = 100;
    requested.bar1_len = 0x100;

    bool ret = mgr.fd != -1;

    if (ret) {
        create_nv_mgr_mdevs(&mgr, NULL, 0, &requested, 1);
        register_nv_mgr_mdevs(&mgr);
    }

    free_nv_mgr(&mgr);

    return ret;
}

int main()
{
    const uint32_t NUM_TESTS = 1;

    const std::string test_names[] = {
        "Create MDevs"
    };
    const std::string test_details[] = {
        "Nvidia manager create mdevs failed.",
    };

    bool (*tests[])(void) = {
        create_mdevs,
    };

    uint32_t failures = 0;

    for (uint32_t i = 0; i < NUM_TESTS; ++i) {
        bool test_result = tests[i]() == 1;

        if (test_result)
            cout << GREEN_COLOR << test_names[i]
                 << " succeeded" RESET_COLOR "\n";
        else {
            ++failures;
            cout << RED_COLOR << test_names[i]
                 << ": " << test_details[i]
                 << RESET_COLOR "\n";
        }
    }

    if (failures == 0)
        cout << GREEN_COLOR << "All tests succeeded" RESET_COLOR "\n";
    else
        cout << RED_COLOR << failures << "/"
             << NUM_TESTS << " tests failed" RESET_COLOR "\n";
}
