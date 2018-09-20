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
#include "rpc_msg.h"


typedef struct msg_box
{
    uint32_t    cmd;
    uint32_t    data;

} msg_box_t;


static msg_box_t        g_msg[RPC_MSG_MAX_QUEUE_SIZE];


static void
_test_rpc_msg(void)
{
    int     i;

    rpc_msg_init(RPC_MSG_ID_CORE0_CORE1);

    for(i = 0; i < RPC_MSG_MAX_QUEUE_SIZE; i++)
    {
        rpc_msg_state_t     state = RPC_MSG_STATE_OK;

        g_msg[i].cmd  = 0xaaaaaaaa;
        g_msg[i].data = i;
        state = rpc_msg_push(RPC_MSG_ID_CORE0_CORE1, &g_msg[i], 0);
        printf("state = %d\n", state);
        printf("\n");
    }


    {
        msg_box_t       *pMsg_box = 0;

        rpc_msg_pop(RPC_MSG_ID_CORE0_CORE1, (void**)&pMsg_box, 0);
    }

    return;
}

int main()
{
    _test_rpc_msg();

    return 0;
}
