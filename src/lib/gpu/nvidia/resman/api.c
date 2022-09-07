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
#include <gpu/nvidia/resman/api.h>
#include <gpu/nvidia/resman/classes.h>
#include <gpu/nvidia/resman/ioctl.h>
#include <gpu/nvidia/resman/types.h>

#include <gpu/nvidia/resources.h>

#include <gpu/mdev.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*! \failure Invalid File Descriptor - This occurs when ctl_fd is -1.
 * \failure Invalid RM Version - This will occur when there is a mismatch on the driver
 *                               and the user suite.
 * \failure Seg Fault - This occurs if version is not pointing to NULL but is pointing to a
 *                      previously allocated memory address.
 */
uint8_t rm_version_check(int ctl_fd, uint8_t ignore_version, const char* version)
{
    if (ctl_fd == -1)
        return 0;

    uint8_t ret = 0;
    struct NvVersionCheck version_check = {};

    if (ignore_version)
        version_check.cmd = 0x32;

    if (version != NULL)
        strncpy(version_check.version, version, sizeof(version_check.version));

    ret = ioctl(ctl_fd, NV_VERSION_CHECK, &version_check) != -1;

    if (ret)
        ret = version_check.reply == 1;

    return ret;
}

/*! \failure Invalid File Descriptor - Occurs when fd is -1.
 * \failure Incorrect File Descriptor - Occurs when fd is an incorrect descriptor.
 * \todo Inform user of status error if there is an error.
 */
struct NvResource* rm_alloc_res(
    int fd,
    struct NvResource* parent,
    uint32_t object,
    uint32_t rm_class,
    void* data
)
{
    struct NvResource* ret = NULL;
    struct RmAllocRes alloc_res = {};

    if (fd == -1)
        return NULL;

    alloc_res.hObjectNew = object;
    alloc_res.hClass = rm_class;
    alloc_res.pAllocParams = data;

    if (parent != NULL) {
        alloc_res.hRoot = parent->client;
        alloc_res.hObjectParent = parent->object;
    }

    if (ioctl(fd, NV_ALLOC_RES, &alloc_res) != -1) {
        if (alloc_res.status == 0x00)
            ret = calloc(1, sizeof(struct NvResource));

        if (ret != NULL) {
            ret->fd = fd;
            ret->object = alloc_res.hObjectNew;
            ret->rm_class = rm_class;
            ret->class_info = data;

            if (parent == NULL) {
                ret->client = alloc_res.hObjectNew;
                ret->parent = alloc_res.hObjectNew;
            } else {
                ret->client = parent->client;
                ret->parent = parent->object;

                struct NvResource* child = parent->child;
                while (child != NULL && child->next != NULL)
                    child = child->next;
                if (child != NULL)
                    child->next = ret;
                else
                    parent->child = ret;
            }
        }

        return ret;
    }

    return NULL;
}

/*! \failure Invalid File Descriptor - Occurs when fd is -1.
 * \failure Incorrect File Descriptor - Occurs when the fd is an incorrect file descriptor.
 */
uint8_t rm_free_res(int fd, struct NvResource* object)
{
    struct RmFreeRes free_res = {};

    if (object == NULL || fd == -1)
        return 0;

    free_res.hRoot = object->client;
    free_res.hObjectParent = object->parent;
    free_res.hObjectOld = object->object;

    return (ioctl(fd, NV_FREE_RES, &free_res) != -1 && free_res.status == 0);
}

void rm_free_tree(int fd, struct NvResource* root)
{
    if (root == NULL)
        return;

    rm_free_tree(fd, root->child);
    rm_free_tree(fd, root->next);

    rm_free_res(fd, root);

    if (root->rm_class == 0x00000080 && root->class_info != NULL) {
        struct Nv0000CtrlGpuDeAttachIdsParams deattach_ids = {};
        deattach_ids.gpu_ids[0] = ((struct Gpu*) root->class_info)->identifier;
        deattach_ids.gpu_ids[1] = 0xFFFFFFFF;
        close(root->fd);
        _RM_CTRL(fd, root->client, root->client, NV0000_DEATTACH_IDS, deattach_ids);
        free(root->class_info);
    }

    free(root);
}

/*! \failure Invalid File Descriptor - Occurs when fd is -1.
 * \failure Incorrect File Descriptor - Occurs when the fd is an incorrect file descriptor.
 * \failure RM Failure - Occurs when incorrect information is placed.
 */
void* rm_ctrl_res(int fd, uint32_t client, uint32_t device, uint32_t command, void* data, uint32_t size)
{
    struct RmControlRes ctrl_res = {};

    if (fd == -1)
        return NULL;

    ctrl_res.client = client;
    ctrl_res.object = device;
    ctrl_res.cmd = command;
    ctrl_res.params = data;
    ctrl_res.param_size = size;

    if (ioctl(fd, NV_CONTROL_RES, &ctrl_res) == -1)
        return NULL;

    if (ctrl_res.status == 0)
        return data;

    /*! \todo Update to use proper logging mechanism (ERROR).
     */
    printf(
        "Failed RM Control Mechanism:\n"
        "\tclient: 0x%.8X\n"
        "\tobject: 0x%.8X\n"
        "\tcmd: 0x%.8X\n"
        "\tflags: 0x%.8X\n"
        "\tparams: %p\n"
        "\tsize: 0x%.8X\n"
        "\tstatus: 0x%.8X\n",
        ctrl_res.client,
        ctrl_res.object,
        ctrl_res.cmd,
        ctrl_res.flags,
        ctrl_res.params,
        ctrl_res.param_size,
        ctrl_res.status
    );

    return NULL;
}

/*! \todo Proper logging if a failure occurs.
 */
uint8_t rm_alloc_os_event(int fd, uint32_t client_id, uint32_t device_id)
{
    struct RmAllocOsEvent event = {};

    event.client = client_id;
    event.device = device_id;
    event.fd = fd;

    if (ioctl(fd, NV_CREATE_OS_EVENT, &event) == -1 || event.status != 0)
        return 0;

    return event.status == 0;
}
