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

#ifndef _clc365_h_
#define _clc365_h_

#ifdef __cplusplus
extern "C" {
#endif

#define ACCESS_COUNTER_NOTIFY_BUFFER (0xc365)

#define NVC365_NOTIFY_BUF
#define NVC365_NOTIFY_BUF_ENTRY                          0x0000001f:0x00000000
#define NVC365_NOTIFY_BUF_SIZE                                              32
#define NVC365_NOTIFY_BUF_ENTRY_TYPE                         MW((0+0*32):(0*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_TYPE_CPU                            0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_TYPE_GPU                            0x00000001
#define NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE                    MW((1+0*32):(0*32+1))
#define NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE_GVA                       0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_ADDR_TYPE_GPA                       0x00000001
#define NVC365_NOTIFY_BUF_ENTRY_BANK                         MW((5+0*32):(0*32+2))
#define NVC365_NOTIFY_BUF_ENTRY_BANK_0                              0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_BANK_1                              0x00000001
#define NVC365_NOTIFY_BUF_ENTRY_BANK_2                              0x00000002
#define NVC365_NOTIFY_BUF_ENTRY_BANK_3                              0x00000003
#define NVC365_NOTIFY_BUF_ENTRY_BANK_4                              0x00000004
#define NVC365_NOTIFY_BUF_ENTRY_BANK_5                              0x00000005
#define NVC365_NOTIFY_BUF_ENTRY_BANK_6                              0x00000006
#define NVC365_NOTIFY_BUF_ENTRY_BANK_7                              0x00000007
#define NVC365_NOTIFY_BUF_ENTRY_BANK_8                              0x00000008
#define NVC365_NOTIFY_BUF_ENTRY_BANK_9                              0x00000009
#define NVC365_NOTIFY_BUF_ENTRY_BANK_10                             0x0000000a
#define NVC365_NOTIFY_BUF_ENTRY_BANK_11                             0x0000000b
#define NVC365_NOTIFY_BUF_ENTRY_BANK_12                             0x0000000c
#define NVC365_NOTIFY_BUF_ENTRY_BANK_13                             0x0000000d
#define NVC365_NOTIFY_BUF_ENTRY_BANK_14                             0x0000000e
#define NVC365_NOTIFY_BUF_ENTRY_BANK_15                             0x0000000f
#define NVC365_NOTIFY_BUF_ENTRY_APERTURE                     MW((9+0*32):(0*32+8))
#define NVC365_NOTIFY_BUF_ENTRY_APERTURE_VID_MEM                    0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_APERTURE_PEER_MEM                   0x00000001
#define NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_COHERENT           0x00000002
#define NVC365_NOTIFY_BUF_ENTRY_APERTURE_SYS_MEM_NONCOHERENT        0x00000003
#define NVC365_NOTIFY_BUF_ENTRY_INST_APERTURE              MW((11+0*32):(0*32+10))
#define NVC365_NOTIFY_BUF_ENTRY_INST_APERTURE_VID_MEM               0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT      0x00000002
#define NVC365_NOTIFY_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT   0x00000003
#define NVC365_NOTIFY_BUF_ENTRY_INST_LO                    MW((31+0*32):(0*32+12))
#define NVC365_NOTIFY_BUF_ENTRY_INST_HI                     MW((31+1*32):(1*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_INST                       MW((31+1*32):(0*32+12))
#define NVC365_NOTIFY_BUF_ENTRY_ADDR_LO                     MW((31+2*32):(2*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_ADDR_HI                     MW((31+3*32):(3*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_ADDR                        MW((31+3*32):(2*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_SUB_GRANULARITY             MW((31+4*32):(4*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_NOTIFY_TAG                  MW((19+5*32):(5*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_COUNTER_VAL                 MW((15+6*32):(6*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_PEER_ID                      MW((2+7*32):(7*32+0))
#define NVC365_NOTIFY_BUF_ENTRY_MMU_ENGINE_ID              MW((28+7*32):(7*32+20))
#define NVC365_NOTIFY_BUF_ENTRY_VALID                      MW((31+7*32):(7*32+31))
#define NVC365_NOTIFY_BUF_ENTRY_VALID_FALSE                         0x00000000
#define NVC365_NOTIFY_BUF_ENTRY_VALID_TRUE                          0x00000001
#define NVC365_NOTIFIERS_ACCESS_COUNTER                  (0)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc365_h_ */
