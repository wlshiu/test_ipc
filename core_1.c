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
/** @file core_1.c
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
typedef ipc_err_t (*CB_IPC_PROC_CMD)(msg_box_t *pMsg_box, void *pUser_data);
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

//=============================================================================
//                  Private Function Definition
//=============================================================================
static ipc_err_t
_ipc_recv_cmd(
    ipc_channel_t       channel,
    msg_box_t           **ppMsg_box,
    uint32_t            cmd_index,
    CB_IPC_PROC_CMD     cb_proc_cmd)
{
    ipc_err_t     rval = IPC_ERR_OK;

    do {
        uint32_t    cmdq_base = 0;
        uint32_t    share_buf_base = 0;
        msg_box_t   *pMsg_box = 0;

        cmdq_base       = (channel == IPC_CHANNEL_0) ? (uint32_t)IPC_CMDQ_BASE_0 : (uint32_t)IPC_CMDQ_BASE_1;
        share_buf_base  = (channel == IPC_CHANNEL_0) ? (uint32_t)IPC_SHARE_BUF_BASE_0 : (uint32_t)IPC_SHARE_BUF_BASE_1;

        if( IPC_CMD_EMPTY(cmdq_base) )
        {
            rval = IPC_ERR_FULL;
            break;
        }

        pMsg_box = (msg_box_t*)IPC_GET_MSG_BOX_SLOT(share_buf_base, cmd_index);
        *ppMsg_box = pMsg_box;

        rval = cb_proc_cmd(pMsg_box, &cmd_index);
        if( rval < 0 )      break;

        BITBAND_SRAM_BIT_CLR(cmdq_base, cmd_index);

    } while(0);

    return rval;
}

static ipc_err_t
_cmd_proc(
    msg_box_t   *pMsg_box,
    void        *pUser_data)
{
    ipc_err_t       rval = IPC_ERR_OK;

    switch( pMsg_box->base.cmd )
    {
        case IPC_CMD_HOLLOW:
            msg("get 'Cmd': %08u, %s\n",
                pMsg_box->hollow.cnt, pMsg_box->hollow.pStr);
            pMsg_box->base.report_rst = 111;
            break;
        default:
            break;
    }

    return rval;
}

static void*
_core_1_proc(void *argv)
{
    uint32_t        cmd_cnt = 0;

    while(1)
    {
        ipc_err_t   rval = IPC_ERR_OK;
        msg_box_t   *pMsg_box = 0;

        rval = _ipc_recv_cmd(IPC_CHANNEL_0, &pMsg_box, cmd_cnt, _cmd_proc);
        if( rval )
        {
            Sleep(10);
            continue;
        }

        cmd_cnt++;

//        Sleep(100);
    }
    pthread_exit(NULL);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
void init_core_1(void)
{
    pthread_t   t1;

    pthread_create(&t1, NULL, &_core_1_proc, NULL);

    return;
}
