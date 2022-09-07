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

#include <gpu/nvidia/device.h>

#include <utils/colors.h>
#include <utils/device.h>

using std::cout;

/*! \page dev-creation-test Device Creation Test
 *
 * \tableofcontents
 *
 * This code is designed to serve as a test bed for determining
 * if the device creation code is working as intended by all
 * components. The following tests are implemented:
 *
 * -# \ref dev-check - Determines if the system has a particular file.
 * -# \ref dev-major-null - Determines the major number for a known device.
 * -# \ref dev-major-ok - Determines the major number for a known device.
 * -# \ref nvidia-tests - Tests specifically designed for determine
 *                        Nvidia GPUs.
 *    -# \ref nvmode - Gets the parameter for DeviceFileMode
 *    -# \ref nvctl - Open the nvidiactl file.
 *    -# \ref nvdev - Open a nvidia\%d file.
 *
 * \section dev-check Device Check Test
 *
 * This test simply checks that the tty device is properly created and
 * attached to the system. The pseudo-code that is executed to determine
 * if this test is a success is:
 *
 * ```{.c}
 * device_exists("/dev/tty") && device_exists("tty")
 * ```
 *
 * \section dev-major-null Device Major Number Identifier Null
 *
 * This test determines if we can correctly select -1 for an invalid number
 * for special device files. The pseudo-code that is executed to determine
 * if this test is a success is:
 *
 * ```{.c}
 * get_major("invalid-gvm") == -1
 * ```
 *
 * \section dev-major-ok Device Major Number Identifier Ok
 *
 * This test determines if we can correctly select the major number
 * for special device files. The pseudo-code that is executed to determine
 * if this test is a success is:
 *
 * ```{.c}
 * get_major("mem") == 1
 * ```
 *
 * \section nvidia-tests Nvidia Specific Tests
 *
 * These tests are specifically designed to test the Nvidia device creations.
 *
 * \subsection nvmode DeviceFileMode Nvidia Check
 *
 * This test determines if the device file mode we get for the nvidia device
 * is structured correctly. The pseudo-code that is executed to determine if
 * this test is a success is:
 *
 * ```{.c}
 * get_param("/proc/driver/nvidia/params", "DeviceFileMode") == 438
 * ```
 *
 * \subsection nvctl Open /dev/nvidiactl File
 *
 * This code determines if we can open the nvidiactl file.
 *
 * NOTE: Creates the file if the file does not exist.
 *
 * \subsection nvdev Open /dev/nvidia File
 *
 * This code determines if we can open the nvidia\%d file.
 *
 * NOTE: Creates the file if the file does not exist.
 *
 */

bool dev_check()
{
    return device_exists("/dev/tty") && device_exists("tty");
}

bool dev_major_null()
{
    return get_major("invalid-gvm") == -1;
}

bool dev_major_ok()
{
    return get_major("mem") == 1;
}

bool nvmode()
{
    return get_param("/proc/driver/nvidia/params", "DeviceFileMode") == 438;
}

bool nvctl()
{
    int fd = nv_open_dev(255);
    bool ret = false;

    if (fd != -1) {
        ret = true;
        close(fd);
    }

    return ret;
}

bool nvdev()
{
    int fd = nv_open_dev(0);
    bool ret = false;

    if (fd != -1) {
        ret = true;
        close(fd);
    }

    return ret;
}

int main()
{
    const uint32_t NUM_TESTS = 6;

    const std::string test_names[] = {
        "Device Check Test",
        "Device Major Number Identifier Null",
        "Device Major Number Identifier",
        "DeviceFileMode Nvidia Check",
        "Open /dev/nvidiactl File",
        "Open /dev/nvidia0 File"
    };
    const std::string test_details[] = {
        "The device \"tty\" and \"/dev/tty\" do not exist on the system.",
        "The device major \"invalid-gvm\" is not equal to -1.",
        "The device major \"mem\" is not equal to 1.",
        "The Nvidia Device File Mode is not equal to 438.",
        "Could not open nvidiactl file, try using sudo to run this test.",
        "Could not open nvidia0 file, try using sudo to run this test."
    };

    bool (*tests[])(void) = {
        dev_check, dev_major_null, dev_major_ok, nvmode,
        nvctl, nvdev
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
