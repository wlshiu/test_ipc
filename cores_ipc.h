/*
 * Copyright (c) 2018-, Wei-Lun Hsu. All Rights Reserved.
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 3 of the
 * License.
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 * See the GNU General Public License version 3 for more details.
 */
/** @file cores_ipc.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/04/25
 * @license
 * @description
 */

#ifndef __cores_ipc_H_wZTBrR1h_lzfz_H7Um_spRH_uNRpqkDbjA81__
#define __cores_ipc_H_wZTBrR1h_lzfz_H7Um_spRH_uNRpqkDbjA81__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//=============================================================================
//                  Constant Definition
//=============================================================================
#define IPC_CMDQ_MAX()                      10 // sizeof(uint32_t)

/* Bit band SRAM definitions */
#define BITBAND_SRAM_REF        0x20000000
#define BITBAND_SRAM_BASE       0x22000000

#define IPC_CMDQ_BASE           (BITBAND_SRAM_REF + 0)
//=============================================================================
//                  Macro Definition
//=============================================================================
#define BITBAND_SRAM(a,b)                   ((BITBAND_SRAM_BASE + (((a)-BITBAND_SRAM_REF)<<5) + ((b)<<2)))  // Convert SRAM address

/* Basic bit band function definitions */
#define BITBAND_SRAM_BIT_CLR(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)) = 0)
#define BITBAND_SRAM_BIT_SET(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)) = 1)
#define BITBAND_SRAM_BIT_GET(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)))


/**
 *  The CmdQ writing direction LSB to MSB
 */

#define IPC_CMD_EMPTY()                     (*((uint32_t*)IPC_CMDQ_BASE) == 0)


//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================
#if 0
#define IPC_CMDQ_READ_IDX()                ((IPC_CMDQ_BASE) & ((IPC_CMDQ_BASE) & (-(IPC_CMDQ_BASE))))
static inline uint32_t IPC_CMD_MSB_IDX(x)
{
    for(int i = 0; i <= 32; ++i)
        x |= (x >> (0x1l << i));

    return (x & ~(x >> 1));
}
#endif
//=============================================================================
//                  Public Function Definition
//=============================================================================

#ifdef __cplusplus
}
#endif

#endif
