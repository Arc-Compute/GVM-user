/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _clc369_h_
#define _clc369_h_

#ifdef __cplusplus
extern "C" {
#endif

#define MMU_FAULT_BUFFER (0xc369)

#define NVC369_BUF
#define NVC369_BUF_ENTRY                                      0x0000001f:0x00000000
#define NVC369_BUF_SIZE                                                          32
#define NVC369_BUF_ENTRY_INST_APERTURE                            MW((9+0*32):(0*32+8))
#define NVC369_BUF_ENTRY_INST_APERTURE_VID_MEM                           0x00000000
#define NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT                  0x00000002
#define NVC369_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT               0x00000003
#define NVC369_BUF_ENTRY_INST_LO                                MW((31+0*32):(0*32+12))
#define NVC369_BUF_ENTRY_INST_HI                                 MW((31+1*32):(1*32+0))
#define NVC369_BUF_ENTRY_INST                                   MW((31+1*32):(0*32+12))
#define NVC369_BUF_ENTRY_ADDR_PHYS_APERTURE                       MW((1+2*32):(2*32+0))
#define NVC369_BUF_ENTRY_ADDR_LO                                MW((31+2*32):(2*32+12))
#define NVC369_BUF_ENTRY_ADDR_HI                                 MW((31+3*32):(3*32+0))
#define NVC369_BUF_ENTRY_ADDR                                   MW((31+3*32):(2*32+12))
#define NVC369_BUF_ENTRY_TIMESTAMP_LO                            MW((31+4*32):(4*32+0))
#define NVC369_BUF_ENTRY_TIMESTAMP_HI                            MW((31+5*32):(5*32+0))
#define NVC369_BUF_ENTRY_TIMESTAMP                               MW((31+5*32):(4*32+0))
#define NVC369_BUF_ENTRY_ENGINE_ID                                MW((8+6*32):(6*32+0))
#define NVC369_BUF_ENTRY_FAULT_TYPE                               MW((4+7*32):(7*32+0))
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT                         MW((7+7*32):(7*32+7))
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT_FALSE                          0x00000000
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT_TRUE                           0x00000001
#define NVC369_BUF_ENTRY_CLIENT                                  MW((14+7*32):(7*32+8))
#define NVC369_BUF_ENTRY_ACCESS_TYPE                            MW((19+7*32):(7*32+16))
#define NVC369_BUF_ENTRY_MMU_CLIENT_TYPE                        MW((20+7*32):(7*32+20))
#define NVC369_BUF_ENTRY_GPC_ID                                 MW((28+7*32):(7*32+24))
#define NVC369_BUF_ENTRY_PROTECTED_MODE                         MW((29+7*32):(7*32+29))
#define NVC369_BUF_ENTRY_PROTECTED_MODE_FALSE                            0x00000000
#define NVC369_BUF_ENTRY_PROTECTED_MODE_TRUE                             0x00000001
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN                    MW((30+7*32):(7*32+30))
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN_FALSE                       0x00000000
#define NVC369_BUF_ENTRY_REPLAYABLE_FAULT_EN_TRUE                        0x00000001
#define NVC369_BUF_ENTRY_VALID                                  MW((31+7*32):(7*32+31))
#define NVC369_BUF_ENTRY_VALID_FALSE                                     0x00000000
#define NVC369_BUF_ENTRY_VALID_TRUE                                      0x00000001
#define NVC369_NOTIFIER_MMU_FAULT_NON_REPLAYABLE                    0
#define NVC369_NOTIFIER_MMU_FAULT_REPLAYABLE                        1
#define NVC369_NOTIFIER_MMU_FAULT_ERROR                             2
#define NVC369_NOTIFIER_MMU_FAULT_NON_REPLAYABLE_IN_PRIV            3

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc369_h_ */
