/*
 * Copyright (C) 2666680 Ontario Inc.
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 */
#ifndef ENVY_H
#define ENVY_H

#ifdef ENVY_LINUX
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/*! \brief Probe structure to interact with Envy Module if it is installed.
 */
struct envy_probe {
    char function[128];
    uint32_t offset;
    uint8_t registers[17];
};

#define ENVY_PROBE_REG 0xDEADBEEF
#define ENVY_PROBE_UNREG 0xE719DEAD
#define ENVY_PROBE_DUMP_START 0xBEEFDEAD
#define ENVY_PROBE_DUMP_STOP 0xDEBEEFED

#define REG_NON 0
#define REG_RAX 1
#define REG_RBX 2
#define REG_RCX 3
#define REG_RDX 4
#define REG_RSI 5
#define REG_RDI 6
#define REG_RBP 7
#define REG_RSP 8
#define REG_R8  9
#define REG_R9  10
#define REG_R10 11
#define REG_R11 12
#define REG_R12 13
#define REG_R13 14
#define REG_R14 15
#define REG_R15 16
#define REG_INV 0xFF

#ifdef ENVY_LINUX
#include <probes.h>
#endif

#endif
