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
/** @file main.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU General Public License version 3
 * @description
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pthread.h"
#include <windows.h>

#include "rpc_binder.h"


typedef struct msg_box
{
    uint32_t    cmd;
    uint32_t    data;

} msg_box_t;


extern void test_rpc_simulation();


static msg_box_t            g_msg[RPC_BINDER_MSG_NUM_MAX];


#define RPC_MSG_CHANNEL_CORE0_2_CORE1       RPC_MSG_CHANNEL_00

static void __attribute__ ((unused))
_test_rpc_msg(void)
{
    int                 i;
    rpc_state_t         state = RPC_STATE_OK;

    rpc_msg_init(RPC_MSG_CHANNEL_CORE0_2_CORE1, RPC_MSG_MAX_QUEUE_SIZE);

    for(i = 0; i < RPC_MSG_MAX_QUEUE_SIZE + 1; i++)
    {
        g_msg[i].cmd  = 0xaaaaaaaa;
        g_msg[i].data = i;
        state = rpc_msg_push(RPC_MSG_CHANNEL_CORE0_2_CORE1, &g_msg[i], 0);
        printf("state = %d\n", state);
        printf("\n");
    }

    for(i = 0; i < RPC_MSG_MAX_QUEUE_SIZE; ++i)
    {
        msg_box_t       *pMsg_box = 0;

        state = rpc_msg_pop(RPC_MSG_CHANNEL_CORE0_2_CORE1, (void**)&pMsg_box, 0);
        printf("state = %d\n", state);
        printf("x%x, %d\n", pMsg_box->cmd, pMsg_box->data);
    }

    return;
}

static void __attribute__ ((unused))
_test_rpc_binder()
{
    #define MSG_BOX_NUM     5
    int                 i;
    rpc_state_t         state = RPC_STATE_OK;
    rpc_msg_box_t       msg_box[MSG_BOX_NUM];

    rpc_binder_init(RPC_MSG_CHANNEL_00, (uint32_t*)&msg_box, sizeof(msg_box));

    for(i = 0; i < RPC_BINDER_MSG_NUM_MAX; i++)
    {
        rpc_msg_box_t       *pCur_box = &msg_box[i % MSG_BOX_NUM];

        pCur_box->procedurse_id     = RPC_BINDER_PROCEDURE_05;
        pCur_box->data.def.param[0] = i;
        pCur_box->data.def.param[1] = 0x1111;
        state = rpc_binder_send(RPC_MSG_CHANNEL_00, pCur_box);
        if( state == RPC_STATE_Q_FULL )
            break;

        printf("%02d-th, state = %d\n", i, state);
    }

    do {
        state = rpc_binder_recv(RPC_MSG_CHANNEL_00);
    } while( state != RPC_STATE_Q_EMPTY );

    return;
}



int main()
{

    srand(time(NULL));
    // _test_rpc_msg();
    // _test_rpc_binder();

    test_rpc_simulation();

    return 0;
}
