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
/** @file core_0.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/04/25
 * @license
 * @description
 */


#include <stdio.h>
#include <windows.h>
#include <pthread.h>
#include "cores_ipc.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
typedef ipc_err_t (*CB_IPC_ASSIGN_CMD)(msg_box_t *pMsg_box, void *pUser_data);
//=============================================================================
//                  Macro Definition
//=============================================================================
#define msg(str, args...)   printf("%s: " str, __func__, ##args);
//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
static uint8_t         g_str_buf[128] = {0};
//=============================================================================
//                  Private Function Definition
//=============================================================================
static ipc_err_t
_ipc_send_cmd(
    ipc_channel_t       channel,
    msg_box_t           **ppMsg_box,
    uint32_t            cmd_index,
    CB_IPC_ASSIGN_CMD   cb_assign_cmd)
{
    ipc_err_t     rval = IPC_ERR_OK;

    do {
        uint32_t    cmdq_base = 0;
        uint32_t    share_buf_base = 0;
        msg_box_t   *pMsg_box = 0;

        cmdq_base       = (channel == IPC_CHANNEL_0) ? (uint32_t)IPC_CMDQ_BASE_0 : (uint32_t)IPC_CMDQ_BASE_1;
        share_buf_base  = (channel == IPC_CHANNEL_0) ? (uint32_t)IPC_SHARE_BUF_BASE_0 : (uint32_t)IPC_SHARE_BUF_BASE_1;
        if( IPC_CMD_FULL(cmdq_base) )
        {
            rval = IPC_ERR_FULL;
            break;
        }

        pMsg_box = (msg_box_t*)IPC_GET_MSG_BOX_SLOT(share_buf_base, cmd_index);
        memset(pMsg_box, 0x0, sizeof(msg_box_t));
        *ppMsg_box = pMsg_box;

        rval = cb_assign_cmd(pMsg_box, &cmd_index);
        if( rval < 0 )      break;

        BITBAND_SRAM_BIT_SET(cmdq_base, cmd_index);

        // synchronous mode,  wait result report
        while( BITBAND_SRAM_BIT_GET(cmdq_base, cmd_index) )
        {
            Sleep(1);
        }
    } while(0);

    return rval;
}

static ipc_err_t
_cmd_assign(
    msg_box_t   *pMsg_box,
    void        *pUser_data)
{
    ipc_err_t       rval = IPC_ERR_OK;
    uint32_t        *pCmd_index = (uint32_t*)pUser_data;

    snprintf((char*)g_str_buf, 128, "%s %08u\n", "HollOW", *pCmd_index);

    pMsg_box->base.cmd      = IPC_CMD_HOLLOW;
    pMsg_box->hollow.cnt    = *pCmd_index;
    pMsg_box->hollow.pStr   = g_str_buf;
    return rval;
}

static void*
_core_0_proc(void *argv)
{
    uint32_t    cmd_cnt = 0;

    while(1)
    {
        ipc_err_t   rval = IPC_ERR_OK;
        msg_box_t   *pMsg_box = 0;

        rval = _ipc_send_cmd(IPC_CHANNEL_0, &pMsg_box, cmd_cnt, _cmd_assign);
        if( rval )
        {
            Sleep(10);
            continue;
        }

        msg("  cmd_end(%08u, %u)\n", cmd_cnt, pMsg_box->base.report_rst);
        cmd_cnt++;

//        Sleep(100);
    }
    pthread_exit(NULL);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
void init_core_0(void)
{
    pthread_t   t0;

    pthread_create(&t0, NULL, &_core_0_proc, NULL);

    return;
}
