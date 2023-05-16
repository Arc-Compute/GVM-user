/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef GVM_NVIDIA_OPEN_SIGNATURE_H
#define GVM_NVIDIA_OPEN_SIGNAUTRE_H

#include <ctrl/ctrla081.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Creates a signature for a vGPU object.
 *
 * This is the create_signature opensource edition, this one uses a
 * hardcoded signature for the moment.
 */
void nvidia_open_create_signature(NVA081_CTRL_VGPU_INFO *info);

#ifdef __cplusplus
};
#endif

#endif
