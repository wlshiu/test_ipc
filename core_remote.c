/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file core_remote.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/03
 * @license
 * @description
 */


#include <windows.h>
#include "core_remote.h"

#include "irq_queue.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================
#if 0
#define trace_enter()                do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][remote] enter\n", __func__, __LINE__); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)

#define trace_leave()                do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][remote] leave\n", __func__, __LINE__); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)
#else
    #define trace_enter()
    #define trace_leave()
#endif


#define msg(str, argv...)           do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][remote] "str, __func__, __LINE__, ##argv); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)
//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
static uint32_t                 g_rpmsg_ready = 0;

static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;

static vring_isr_info_t         g_vring_isr[2] = {0};

extern queue_handle_t      g_vring_irq_q_0;
extern queue_handle_t      g_vring_irq_q_1;
//=============================================================================
//                  Private Function Definition
//=============================================================================

static void
_isr_core_remote(void)
{
//    uint32_t                        vect_id = (uint32_t)-1;
//
//    trace_enter();
//    queue_pop(&g_vring_irq_q_0, (int*)&vect_id);
//
//    if( vect_id < VRING_ISR_COUNT )
//    {
//        vring_isr_info_t    *pInfo = &g_vring_isr[vect_id];
//
//        if( pInfo->vring_isr )
//            pInfo->vring_isr(vect_id, pInfo->data);
//
//        trace_leave();
//    }
//    return;
}

static void*
_task_core_remote(void *argv)
{
    core_attr_t         *pAttr_core = (core_attr_t*)argv;

    pthread_detach(pthread_self());

    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);


    while(1)
    {
//        if( !g_rpmsg_ready )
//            continue;

        Sleep((rand() >> 5)& 0x3);
    }
    pthread_exit(0);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int
core_remote_init(
    core_attr_t     *pAttr_core)
{
    g_rpmsg_ready = 0;

    pAttr_core->pf_irs = _isr_core_remote;
    return 0;
}


int
core_remote_boot(
    core_attr_t     *pAttr)
{
    pthread_t   t;
    pthread_create(&t, 0, _task_core_remote, pAttr);
    return 0;
}

