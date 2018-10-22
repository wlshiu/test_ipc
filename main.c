/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file main.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/19
 * @license
 * @description
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "core_master.h"
#include "core_remote.h"

#include "irq_queue.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
core_attr_t         g_core_attr[CORE_ID_TOTAL];

queue_handle_t      g_vring_irq_q_0;
queue_handle_t      g_vring_irq_q_1;

pthread_mutex_t      g_log_mtx;

uint32_t         g_share_mem[500 << 10] = {0};
//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int main()
{
    srand(clock());

    memset(&g_core_attr, 0x0, sizeof(g_core_attr));

    queue_init(&g_vring_irq_q_0);
    queue_init(&g_vring_irq_q_1);
    pthread_mutex_init(&g_log_mtx, 0);

    //--------------------------------
    // assign basic info
    g_core_attr[CORE_ID_MASTER].core_id     = CORE_ID_MASTER;
    g_core_attr[CORE_ID_REMOTE_1].core_id   = CORE_ID_REMOTE_1;

    //---------------------------------------
    // initialize cores
    core_master_init(&g_core_attr[CORE_ID_MASTER]);
    core_remote_init(&g_core_attr[CORE_ID_REMOTE_1]);

    core_irq_simulator(&g_core_attr[CORE_ID_MASTER], CORE_ID_TOTAL);

    //------------------------------
    // vring
    g_core_attr[CORE_ID_MASTER].pVring_tx_q   = &g_vring_irq_q_0;
    g_core_attr[CORE_ID_MASTER].pVring_rx_q   = &g_vring_irq_q_1;
    g_core_attr[CORE_ID_REMOTE_1].pVring_tx_q = &g_vring_irq_q_1;
    g_core_attr[CORE_ID_REMOTE_1].pVring_rx_q = &g_vring_irq_q_0;

    //------------------------------
    // start
    core_master_boot(&g_core_attr[CORE_ID_MASTER], CORE_ID_TOTAL);

    while(1)    Sleep((DWORD)-1);
    return 0;
}
