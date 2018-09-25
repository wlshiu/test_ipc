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
/** @file test_simulation.c
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



//=============================================================================
//                  Constant Definition
//=============================================================================
#define MSG_BOX_NUM_MAX         5

typedef enum msg_chnnl
{
    MSG_CHNNL_TX        = 0,
    MSG_CHNNL_RX,
    MSG_CHNNL_TOTAL

} msg_chnnl_t;

//=============================================================================
//                  Macro Definition
//=============================================================================
#define DECLARE_PRECEDURSE_SERVICE(id)                              \
    static void                                                     \
    _procedurse_server_##id (                                       \
        rpc_msg_box_t *pBox, void *pTunnel_info) {                  \
        pthread_mutex_lock(&g_log_mtx);                             \
        fprintf((pBox->data.debug.core_id)                          \
                ? g_pfile[CORE_ID_1] : g_pfile[CORE_ID_0],          \
            "[%s][core %d, x%x] procedurse_id= %X, num= %x \n",     \
            __func__,                                               \
            pBox->data.debug.core_id, (uint32_t)pTunnel_info,       \
            pBox->procedurse_id,                                    \
            pBox->data.debug.sequence_num                           \
        );                                                          \
        pthread_mutex_unlock(&g_log_mtx);                           \
    }
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum core_id
{
    CORE_ID_0       = 0,
    CORE_ID_1,
    CORE_ID_TOTAL,

} core_id_t;

typedef void (*CB_IRQ)(void);
typedef struct irq_info
{
    pthread_cond_t      cond;
    CB_IRQ              cb_irq;

} irq_info_t;

typedef struct core_info
{
    core_id_t       core_id;
    uint32_t        channel_id[MSG_CHNNL_TOTAL];

    irq_info_t      *pInfo_irq;

} core_info_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static FILE                 *g_pfile[CORE_ID_TOTAL];
static pthread_mutex_t      g_log_mtx;
static irq_info_t           g_irq_info[CORE_ID_TOTAL];
static core_info_t          g_core_info[CORE_ID_TOTAL];


DECLARE_PRECEDURSE_SERVICE(0);
DECLARE_PRECEDURSE_SERVICE(1);
DECLARE_PRECEDURSE_SERVICE(2);
DECLARE_PRECEDURSE_SERVICE(3);
DECLARE_PRECEDURSE_SERVICE(4);
DECLARE_PRECEDURSE_SERVICE(5);
DECLARE_PRECEDURSE_SERVICE(6);
DECLARE_PRECEDURSE_SERVICE(7);
DECLARE_PRECEDURSE_SERVICE(8);
DECLARE_PRECEDURSE_SERVICE(9);
DECLARE_PRECEDURSE_SERVICE(A);
DECLARE_PRECEDURSE_SERVICE(B);
DECLARE_PRECEDURSE_SERVICE(C);
DECLARE_PRECEDURSE_SERVICE(D);
DECLARE_PRECEDURSE_SERVICE(E);
DECLARE_PRECEDURSE_SERVICE(F);

//=============================================================================
//                  Private Function Definition
//=============================================================================
static void
IPC_IRQHandler_core0(void)
{
    return;
}

static void
IPC_IRQHandler_core1(void)
{
    return;
}

static void*
_task_irq_trigger(void *argv)
{
    pthread_mutex_t         irq_mtx;
    irq_info_t              *pInfo_irq = (irq_info_t*)argv;

    pthread_mutex_init(&irq_mtx, NULL);

    while(1)
    {
        pthread_mutex_lock(&irq_mtx);
        pthread_cond_wait(&pInfo_irq->cond, &irq_mtx);


        if( pInfo_irq->cb_irq )
            pInfo_irq->cb_irq();

        pthread_mutex_unlock(&irq_mtx);
    }

    pthread_exit(NULL);
    return 0;
}

static int
_send_signal(void *pTunnel_info)
{
    #if 0
    // ARM trigger IRQ to other cores
    asm volatile ("dsb");
    asm volatile ("sev");
    #else
    irq_info_t              *pInfo_irq = (irq_info_t*)pTunnel_info;

    pthread_cond_signal(&pInfo_irq->cond);
    #endif
    return 0;
}




//=============================================================================
//                  Public Function Definition
//=============================================================================
static void* __attribute__ ((unused))
_task_core(void *argv)
{
    uint32_t                cnt = 0;
    core_info_t             *pInfo_core = (core_info_t*)argv;
    rpc_msg_channel_t       channel_id[2];
    rpc_msg_box_t           msg_box[MSG_BOX_NUM_MAX];

    channel_id[MSG_CHNNL_TX] = pInfo_core->channel_id[MSG_CHNNL_TX];
    channel_id[MSG_CHNNL_RX] = pInfo_core->channel_id[MSG_CHNNL_RX];

    rpc_binder_init(channel_id[MSG_CHNNL_TX], (uint32_t*)&msg_box, sizeof(msg_box));

    rpc_binder_register_send_handler(channel_id[MSG_CHNNL_TX],
                                     _send_signal, pInfo_core->pInfo_irq);

    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_00, _procedurse_server_0, (void*)0x0);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_01, _procedurse_server_1, (void*)0x1);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_02, _procedurse_server_2, (void*)0x2);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_03, _procedurse_server_3, (void*)0x3);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_04, _procedurse_server_4, (void*)0x4);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_05, _procedurse_server_5, (void*)0x5);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_06, _procedurse_server_6, (void*)0x6);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_07, _procedurse_server_7, (void*)0x7);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_08, _procedurse_server_8, (void*)0x8);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_09, _procedurse_server_9, (void*)0x9);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_10, _procedurse_server_A, (void*)0xA);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_11, _procedurse_server_B, (void*)0xB);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_12, _procedurse_server_C, (void*)0xC);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_13, _procedurse_server_D, (void*)0xD);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_14, _procedurse_server_E, (void*)0xE);
    rpc_binder_register_procedure(RPC_BINDER_PROCEDURE_15, _procedurse_server_F, (void*)0xF);

    while(1)
    {
        // rpc_state_t         state = RPC_STATE_OK;
        rpc_msg_box_t       *pCur_box = &msg_box[cnt % MSG_BOX_NUM_MAX];

        // pop
        rpc_binder_recv(channel_id[MSG_CHNNL_RX]);

        // push
        if( rpc_binder_is_full(channel_id[MSG_CHNNL_TX]) )
            continue;

        pCur_box->procedurse_id           = cnt % RPC_BINDER_PROCEDURE_TOTAL;
        pCur_box->data.debug.core_id      = pInfo_core->core_id;
        pCur_box->data.debug.sequence_num = cnt;
        rpc_binder_send(channel_id[MSG_CHNNL_TX], pCur_box);

        cnt++;

        if( cnt == 0xff)    break;

        Sleep((rand() >> 5) & 0x1);
    }

    pthread_exit(NULL);
    return 0;
}


void __attribute__ ((unused))
test_rpc_simulation()
{

    g_pfile[CORE_ID_0] = fopen("core0.log", "w");
    g_pfile[CORE_ID_1] = fopen("core1.log", "w");


    g_irq_info[CORE_ID_0].cb_irq = IPC_IRQHandler_core0;
    g_irq_info[CORE_ID_1].cb_irq = IPC_IRQHandler_core1;


    pthread_mutex_init(&g_log_mtx, 0);

    {
        pthread_t   t1, t2, t3;

        // core 0
        g_core_info[CORE_ID_0].core_id                  = CORE_ID_0;
        g_core_info[CORE_ID_0].pInfo_irq                = &g_irq_info[CORE_ID_0];
        g_core_info[CORE_ID_0].channel_id[MSG_CHNNL_TX] = RPC_MSG_CHANNEL_00;
        g_core_info[CORE_ID_0].channel_id[MSG_CHNNL_RX] = RPC_MSG_CHANNEL_01;
        pthread_create(&t1, 0, _task_core, &g_core_info[CORE_ID_0]);

        // core 1
        g_core_info[CORE_ID_1].core_id                  = CORE_ID_1;
        g_core_info[CORE_ID_1].pInfo_irq                = &g_irq_info[CORE_ID_1];
        g_core_info[CORE_ID_1].channel_id[MSG_CHNNL_TX] = RPC_MSG_CHANNEL_01;
        g_core_info[CORE_ID_1].channel_id[MSG_CHNNL_RX] = RPC_MSG_CHANNEL_00;
        pthread_create(&t2, 0, _task_core, &g_core_info[CORE_ID_1]);


        pthread_cond_init(&g_irq_info[CORE_ID_0].cond, NULL);
        pthread_cond_init(&g_irq_info[CORE_ID_1].cond, NULL);
        pthread_create(&t3, 0, _task_irq_trigger, &g_irq_info[CORE_ID_0]);
        pthread_create(&t3, 0, _task_irq_trigger, &g_irq_info[CORE_ID_1]);


        pthread_join(t1, 0);
        pthread_join(t2, 0);

    }


    if( g_pfile[CORE_ID_0] )    fclose(g_pfile[CORE_ID_0]);
    if( g_pfile[CORE_ID_1] )    fclose(g_pfile[CORE_ID_1]);


//    while(1)        Sleep(10000);
}

