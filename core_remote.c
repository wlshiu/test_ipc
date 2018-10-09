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
#include "rpmsg/rpmsg.h"
#include "common/hil/hil.h"
#include "porting/platform_info.h"

#include "irq_queue.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define LOCAL_VRING_VECT_TX     VRING_VECT_1
#define LOCAL_VRING_VECT_RX     VRING_VECT_0
//=============================================================================
//                  Macro Definition
//=============================================================================
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

static vring_isr_info_t         g_vring_isr[VRING_VECT_TOTAL] = {0};

extern queue_handle_t      g_irq_queue_0;
extern queue_handle_t      g_irq_queue_1;
//=============================================================================
//                  Private Function Definition
//=============================================================================
static int
_enable_interrupt(
    struct proc_vring   *vring_hw)
{
    int     vect_id = vring_hw->intr_info.vect_id;

    if( vect_id < VRING_VECT_TOTAL )
    {
        g_vring_isr[vect_id].data      = vring_hw;
        g_vring_isr[vect_id].vring_isr = platform_vring_isr;
    }
    return 0;
}

static void
_notify(
    int                 cpu_id,
    struct proc_intr    *intr_info)
{
    /* Trigger IPI (inter-processor interrupt) */
    do {
        core_attr_t     *pAttr_core = (core_attr_t*)intr_info->data;

        msg("%s, %d\n", "enter", intr_info->vect_id);
        queue_push(&g_irq_queue_1, intr_info->vect_id);
        pthread_cond_signal(pAttr_core->pCores_irq_cond);
        msg("%s\n", "leave");
    } while(0);
    return;
}

static struct hil_platform_ops  g_core1_proc_ops =
{
    .enable_interrupt     = _enable_interrupt,
    .reg_ipi_after_deinit = 0,
    .get_status           = 0,
    .set_status           = 0,
    .notify               = _notify,
    .boot_cpu             = 0,
    .shutdown_cpu         = 0,
};


//---------------------------------------------
static void
_rpmsg_recv_cb(
    struct rpmsg_channel    *rp_chnl,
    void                    *data,
    int                     len,
    void                    *priv,
    unsigned long           src)
{
    (void)priv;
    (void)src;
#if 0
    /* On reception of a shutdown we signal the application to terminate */
    if ((*(unsigned int *)data) == SHUTDOWN_MSG)
    {
        evt_chnl_deleted = 1;
        return;
    }
#else
    /* Send data back to master */
    if (rpmsg_send(rp_chnl, data, len) < 0)
    {
        err("%s\n", "rpmsg_send failed");
    }

    g_rpmsg_ready = 1;
    msg("%s\n", "enter");
#endif

    return;
}

static void
_rpmsg_channel_created(
    struct rpmsg_channel    *rp_chnl)
{
    (void)rp_chnl;
    msg("%s\n", "enter");
    return;
}

static void
_rpmsg_channel_deleted(
    struct rpmsg_channel    *rp_chnl)
{
    (void)rp_chnl;
    msg("%s\n", "enter");
    return;
}

//static void
//_isr_core_master(void)
//{
//    return;
//}

static void
_isr_core_remote(void)
{
//    pthread_cond_signal(&g_cond_rpmsg_rx);

    uint32_t                        vect_id = (uint32_t)-1;

    msg("%s\n", "enter");
    queue_pop(&g_irq_queue_0, (int*)&vect_id);

    if( vect_id < VRING_ISR_COUNT )
    {
        vring_isr_info_t    *pInfo = &g_vring_isr[vect_id];

        if( pInfo->vring_isr )
            pInfo->vring_isr(vect_id, pInfo->data);

        msg("%s\n", "leave");
    }
    return;
}

static void*
_task_core_remote(void *argv)
{
    core_attr_t         *pAttr_core = (core_attr_t*)argv;
    remote_device_t     *pRDev = 0;

    pthread_detach(pthread_self());

    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);

    platform_set_ops(pAttr_core->core_id, &g_core1_proc_ops);
    platform_set_vring_intr_priv_data(pAttr_core->core_id, pAttr_core);

    rpmsg_init(RPMSG_MASTER,
               &pRDev,
               _rpmsg_channel_created,
               _rpmsg_channel_deleted,
               _rpmsg_recv_cb,
               RPMSG_REMOTE);

    while(1)
    {
//        pthread_mutex_lock(&g_mtx_rpmsg_rx);
//        pthread_cond_wait(&g_cond_rpmsg_rx, &g_mtx_rpmsg_rx);

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

