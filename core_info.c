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

    pthread_detach(pthread_self());

    while(1)
    {
        int     value = 0;

        if( !queue_pop(&pAttr_core->irq_q, &value) )
        {
            if( pAttr_core->pf_irs )
                pAttr_core->pf_irs();
        }

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

    queue_init(&pAttr_core_0->irq_q);
    queue_init(&pAttr_core_1->irq_q);

    pAttr_core_0->pRemote_irq_q = &pAttr_core_1->irq_q;
    pAttr_core_1->pRemote_irq_q = &pAttr_core_0->irq_q;

    pthread_create(&t, 0, _task_irq, pAttr_core_0);
    pthread_create(&t, 0, _task_irq, pAttr_core_1);

    return 0;
}


