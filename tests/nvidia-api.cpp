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

#include <gpu/nvidia/resman/api.h>
#include <gpu/nvidia/resman/classes/nv0000.h>
#include <gpu/nvidia/device.h>

#include <utils/colors.h>
#include <utils/device.h>

using std::cout;

/*! \page nvidia-api-test NVIDIA API Test
 *
 * \tableofcontents
 *
 * In order to properly test the device's interaction with the RM Core on the server. The components of the RM API
 * that we implemented in the GVM suite are the following:
 *
 * \section RM API Implemented
 *
 * -# NV_CREATE_OS_EVENT - Creates an operating system event.
 * -# NV_VERSION_CHECK - Performs the RM API version check.
 * -# NV_ALLOC_RES - Allocates a resource/client on the system.
 * -# NV_FREE_RES - Frees a resource/client on the system.
 * -# NV_CONTROL_RES - Sends a simple control command for a specific resource/client on the system.
 *
 * These API implementations, allow us to implement the vast majority of the GVM user suite required
 * to provide mediated devices to individuals using the GVM management layers.
 *
 * \section Tests
 *
 * To ensure we can correctly handle the entire system, we developed several tests to ensure all components of the
 * RM API which we use functions correctly:
 *
 * -# NV_VERSION_CHECK Tests
 *    -# \ref version-check-ok - Checks if we can handle the correct version check for the RMCore.
 *    -# \ref version-check-bad - Checks if we can handle the incorrect version check for the RMCore.
 *    -# \ref version-check-ignore - Checks if we can handle the ignore version check for the RMCore.
 * -# NV_ALLOC_RES Tests
 *    -# \ref alloc-root-ok - Checks if we can correctly allocate a root.
 *    -# \ref alloc-root-invalid-fd - Checks if we can correctly fail on allocating a root.
 *    -# \ref alloc-root-bad-fd - Checks if we can correctly fail on allocating a root.
 *    -# \ref alloc-root-bad-arg - Checks if we can correctly fail on allocating a root.
 * -# NV_FREE_RES Tests
 *    -# \ref free-root-ok - Checks if we can correctly deallocate a root object.
 *    -# \ref free-root-invalid-fd - Checks if we can correctly fail on deallocating a root.
 *    -# \ref free-root-double-deallocate - Checks if we can correctly fail on double deallocating a root.
 * -# NV_CONTROL_RES Tests
 *    -# \ref get-probed-ids - Determines if we can get the probed ids correctly.
 *
 * \subsection version-checks Version Check Tests
 *
 * These sections determines if the version check of the RM Core is handled correctly.
 *
 * \subsubsection version-check-ok RM Version Check Ok
 *
 * This code determines if a correct RM Version number works on the system. The corresponding pseudo-code is as
 * follows:
 *
 * ```{.c}
 * rm_version_check(nvctl_file, false, RM_VERSION)
 * ```
 *
 * \subsubsection version-check-bad RM Version Check Incorrect
 *
 * This code determines if an incorrect RM Version number works on the system. The corresponding pseudo-code is as
 * follows:
 *
 * ```{.c}
 * rm_version_check(nvctl_file, false, NULL)
 * ```
 *
 * \subsubsection version-check-ignore RM Version Check Ignore
 *
 * This code determines if an ignored RM Version number works on the system. The corresponding pseudo-code is as
 * follows:
 *
 * ```{.c}
 * rm_version_check(nvctl_file, true, NULL)
 * ```
 *
 * \subsection alloc-root Alloc Root Tests
 *
 * These tests determine if we can correctly allocate client ids.
 *
 * \subsubsection alloc-root-ok Allocates a Client ID.
 *
 * This code determines if we can correctly allocate a device client id for a control file. The corresponding
 * pseudo-code is as follows:
 *
 * ```{.c}
 * rm_alloc_res(ctlfd, NULL, 0, 0, NULL) != NULL
 * ```
 *
 * \subsubsection alloc-root-invalid-fd Allocates a Client ID (Invalid FD)
 *
 * This code determines if we can correctly fail given an invalid FD. The corresponding pseudo-code is as follows:
 *
 * ```{.c}
 * rm_alloc_res(-1, NULL, 0, 0, NULL) == NULL
 * ```
 *
 * \subsubsection alloc-root-bad-fd Allocates a Client ID (Bad FD)
 *
 * This code determines if we can correctly fail given an bad FD. The corresponding pseudo-code is as follows:
 *
 * ```{.c}
 * rm_alloc_res(STDOUT_FILENO, NULL, 0, 0, NULL) == NULL
 * ```
 *
 * \subsubsection alloc-root-bad-arg Allocates a Client Id (Bad Argument)
 *
 * This code determines if we can correctly fail given an bad argument. The corresponding pseudo-code is as follows:
 *
 * ```{.c}
 * rm_alloc_res(ctlfd, NULL, 0, 0xA080, NULL) == NULL
 * ```
 *
 * \subsection free-root Free Root Tests
 *
 * These tests verify we can correctly free the client ids when necessary.
 *
 * \subsubsection free-root-ok Deallocates a Client ID.
 *
 * This code determines if we can correctly deallocate a device client id for a control file. The corresponding
 * pseudo-code is as follows:
 *
 * ```{.c}
 * rm_free_res(ctlfd, resource)
 * ```
 *
 * \subsubsection free-root-invalid-fd Deallocates a Client ID (Invalid FD)
 *
 * This code determines if we can correctly fail given an invalid FD. The corresponding pseudo-code is as follows:
 *
 * ```{.c}
 * rm_free_res(-1, object) == 0
 * ```
 *
 * \subsubsection free-root-double-deallocate Double Deallocation Root
 *
 * This code determines if we properly handle if a resource will be doubly deallocated. The corresponding pseudo-code
 * is:
 *
 * ```{.c}
 * rm_free_res(ctlfd, resource) && !rm_free_res(ctlfd, resource)
 * ```
 *
 * \subsection control Control Tests
 *
 * These tests verify we can correctly control a particular gpu object.
 *
 * \subsubsection get-probed-ids Gets The Probed IDs
 *
 * This code ensures we can get the probed device ids that are present in the NVIDIA kernel module correctly in the
 * user program. The corresponding pseudo-code is:
 *
 * ```{.c}
 * RM_CTRL(ctlfd, ptr, NV0000_GET_PROBED_IDS, probed_ids) != NULL && probed_ids.gpu_ids[0] != 0xFFFFFFFF
 * ```
 */

bool version_check_ok()
{
    int nvctl_fd = nv_open_dev(255);

    bool ret = rm_version_check(nvctl_fd, false, RM_VERSION);

    if (nvctl_fd != -1)
        close(nvctl_fd);

    return ret;
}

bool version_check_bad()
{
    int nvctl_fd = nv_open_dev(255);

    bool ret = rm_version_check(nvctl_fd, false, NULL);

    if (nvctl_fd != -1)
        close(nvctl_fd);

    return ret;
}

bool version_check_ignore()
{
    int nvctl_fd = nv_open_dev(255);

    bool ret = rm_version_check(nvctl_fd, true, NULL);

    if (nvctl_fd != -1)
        close(nvctl_fd);

    return ret;
}

bool alloc_root_ok()
{
    int ctlfd = nv_open_dev(255);

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = ptr != NULL;

    if (ret)
        free(ptr);

    close(ctlfd);

    return ret;
}

bool alloc_root_invalid_fd()
{
    int ctlfd = -1;

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = ptr == NULL;

    if (!ret)
        free(ptr);

    return ret;
}

bool alloc_root_bad_fd()
{
    int ctlfd = STDOUT_FILENO;

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = ptr == NULL;

    if (!ret)
        free(ptr);

    return ret;
}

bool alloc_root_bad_arg()
{
    int ctlfd = nv_open_dev(255);

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0xA080, NULL);

    bool ret = ptr == NULL;

    if (!ret)
        free(ptr);

    close(ctlfd);

    return ret;
}

bool free_root_ok()
{
    int ctlfd = nv_open_dev(255);

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = rm_free_res(ctlfd, ptr);

    if (ptr != NULL)
        free(ptr);

    close(ctlfd);

    return ret;
}

bool free_root_invalid_fd()
{
    int ctlfd = nv_open_dev(255);

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = !rm_free_res(-1, ptr);

    if (ptr != NULL)
        free(ptr);

    close(ctlfd);

    return ret;
}

bool free_root_double_deallocate()
{
    int ctlfd = nv_open_dev(255);

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = rm_free_res(ctlfd, ptr) && !rm_free_res(ctlfd, ptr);

    if (ptr != NULL)
        free(ptr);

    close(ctlfd);

    return ret;
}

bool get_probed_ids()
{
    int ctlfd = nv_open_dev(255);

    struct Nv0000CtrlGpuGetProbedIdsParams probed_ids = {};

    struct NvResource *ptr = rm_alloc_res(ctlfd, NULL, 0, 0, NULL);

    bool ret = false;

    if (ptr != NULL) {
        ret = RM_CTRL(ctlfd, ptr, NV0000_GET_PROBED_IDS, probed_ids) != NULL && probed_ids.gpu_ids[0] != 0xFFFFFFFF;
        rm_free_res(ctlfd, ptr);
        free(ptr);
    }

    close(ctlfd);
    return ret;
}

int main()
{
    const uint32_t NUM_TESTS = 11;

    const std::string test_names[] = {
        "RM Version Check Ok",
        "RM Version Check Incorrect",
        "RM Version Check Ignore",
        "RM Alloc Root Ok",
        "RM Alloc Root Fail (Invalid FD)",
        "RM Alloc Root Fail (Bad FD)",
        "RM Alloc Root Fail (Bad Argument)",
        "RM Free Root Ok",
        "RM Free Root Fail (Invalid FD)",
        "RM Free Root Fail (Double Deallocate)",
        "Get Probed Ids"
    };
    const std::string test_details[] = {
        "Ensure your version check is correctly: " RM_VERSION,
        "Invalid version is reported correctly, please check your driver version.",
        "Ignored version is reported incorrectly, please check your driver version.",
        "RM Alloc Root failed, check driver version.",
        "RM Alloc Root succeeded, SHOULD NOT BE POSSIBLE.",
        "RM Alloc Root succeeded, SHOULD NOT BE POSSIBLE.",
        "RM Alloc Root succeeded, SHOULD NOT BE POSSIBLE.",
        "RM Free Root failed, check driver version.",
        "RM Free Root succeded, SHOULD NOT BE POSSIBLE.",
        "RM Free Root succeded, SHOULD NOT BE POSSIBLE.",
        "No GPU Ids were found."
    };

    bool (*tests[])(void) = {
        version_check_ok,
        version_check_bad,
        version_check_ignore,
        alloc_root_ok,
        alloc_root_invalid_fd,
        alloc_root_bad_fd,
        alloc_root_bad_arg,
        free_root_ok,
        free_root_invalid_fd,
        free_root_double_deallocate,
        get_probed_ids
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
