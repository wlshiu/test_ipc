/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file core_info.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/01
 * @license
 * @description
 */


#include <windows.h>
#include "core_info.h"

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

//=============================================================================
//                  Private Function Definition
//=============================================================================
static void*
_task_irq(void *argv)
{
    core_attr_t             *pAttr_core = (core_attr_t*)argv;
    pthread_mutex_t         irq_mtx;

    pthread_cond_init(&pAttr_core->cond, NULL);
    pthread_mutex_init(&irq_mtx, NULL);

    pthread_detach(pthread_self());

    while(1)
    {
        pthread_mutex_lock(&irq_mtx);
        pthread_cond_wait(&pAttr_core->cond, &irq_mtx);

        if( pAttr_core->pf_irs )
            pAttr_core->pf_irs();

        pthread_mutex_unlock(&irq_mtx);

        Sleep((rand() >> 5) & 0x1);
    }

    pthread_exit(0);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int
core_irq_simulator(
    core_attr_t     *pAttr,
    uint32_t        core_num)
{
    pthread_t        t;
    core_attr_t     *pAttr_core_0 = pAttr;
    core_attr_t     *pAttr_core_1 = pAttr + 1;

    pthread_create(&t, 0, _task_irq, pAttr_core_0);
    pthread_create(&t, 0, _task_irq, pAttr_core_1);

    return 0;
}


