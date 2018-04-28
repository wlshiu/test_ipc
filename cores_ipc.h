/*
 * Copyright (c) 2018-, Shiu. All Rights Reserved.
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

#define SIMULATION_TEST

#if defined(SIMULATION_TEST)
extern uint32_t g_share_buf;
#define BITBAND_SRAM_REF        ((uint8_t*)&g_share_buf)
#define BITBAND_SRAM_BASE       ((uint8_t*)&g_share_buf)
#else   // #if defined(SIMULATION_TEST)

/* Bit band SRAM definitions */
#define BITBAND_SRAM_REF        0x20000000
#define BITBAND_SRAM_BASE       0x22000000
#endif // #if defined(SIMULATION_TEST)


#define ICP_CMD_ARGV_NUM                    10

#define IPC_CMDQ_BASE_0                     (BITBAND_SRAM_REF)
#define IPC_SHARE_BUF_BASE_0                (IPC_CMDQ_BASE_0 + sizeof(uint32_t))

#define IPC_CMDQ_BASE_1                     (IPC_SHARE_BUF_BASE_0 + (IPC_CMDQ_MAX)*sizeof(msg_box_t))
#define IPC_SHARE_BUF_BASE_1                (IPC_CMDQ_BASE_1 + sizeof(uint32_t))

#define IPC_CMDQ_NUM_POW_2                  2 // MUST be less than 5
#define IPC_CMDQ_MAX                        (0x1ul << IPC_CMDQ_NUM_POW_2) // sizeof(uint32_t)


typedef enum ipc_err
{
    IPC_ERR_CMD_RECEIVED        = 4,
    IPC_ERR_CMD_SENT            = 3,
    IPC_ERR_EMPTY               = 2,
    IPC_ERR_FULL                = 1,
    IPC_ERR_OK                  = 0,
    IPC_ERR_NULL_POINTER        = -1,
    IPC_ERR_INVALID_PARAM       = -2,
    IPC_ERR_UNKNOWN             = -3,

} ipc_err_t;

typedef enum ipc_channel
{
    IPC_CHANNEL_0   = 1,
    IPC_CHANNEL_1,

} ipc_channel_t;

typedef enum    ipc_cmd
{
    IPC_CMD_UNKNOWN     = 0,
    IPC_CMD_DUMMY,
    IPC_CMD_HOLLOW,

} ipc_cmd_t;
//=============================================================================
//                  Macro Definition
//=============================================================================
#define BITBAND_SRAM(a,b)                   ((BITBAND_SRAM_BASE + (((a)-BITBAND_SRAM_REF)<<5) + ((b)<<2)))  // Convert SRAM address

#if defined(SIMULATION_TEST)
extern pthread_mutex_t      g_mtx;
#define BITBAND_SRAM_BIT_CLR(a,b)           do{ pthread_mutex_lock(&g_mtx);                     \
                                                *(volatile uint32_t*)(a) &= (~(0x1UL << (b)));  \
                                                pthread_mutex_unlock(&g_mtx);                   \
                                            }while(0)

#define BITBAND_SRAM_BIT_SET(a,b)           do{ pthread_mutex_lock(&g_mtx);                     \
                                                *(volatile uint32_t*)(a) |= (0x1UL << (b));     \
                                                pthread_mutex_unlock(&g_mtx);                   \
                                            }while(0)

#define BITBAND_SRAM_BIT_GET(a,b)           (*(volatile uint32_t*)(a) & (0x1ul << (b)))

#else   // #if defined(SIMULATION_TEST)

/* Basic bit band function definitions */
#define BITBAND_SRAM_BIT_CLR(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)) = 0)
#define BITBAND_SRAM_BIT_SET(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)) = 1)
#define BITBAND_SRAM_BIT_GET(a,b)           (*(volatile uint32_t*) (BITBAND_SRAM(a,b)))
#endif // #if defined(SIMULATION_TEST)




/**
 *  The CmdQ writing direction LSB to MSB
 */
#define IPC_CMD_EMPTY(pAddr)                (*((volatile uint32_t*)(pAddr)) == 0)
#define IPC_CMD_FULL(pAddr)                 (*((volatile uint32_t*)(pAddr)) == (IPC_CMDQ_MAX-1))
#define IPC_GET_MSG_BOX_SLOT(pShare, idx)   ((pShare) + ((idx) & (IPC_CMDQ_MAX-1)) * sizeof(msg_box_t))

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct  msg_box_base
{
    ipc_cmd_t       cmd;
    ipc_err_t       report_rst;
} msg_box_base_t;

typedef struct  msg_box
{
    msg_box_base_t      base;

    union {
        struct {
            uint32_t        cnt;
            char            *pStr;
        } hollow;

        struct {
            uint32_t        argv[ICP_CMD_ARGV_NUM];
        } def;
    };
} msg_box_t;
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
