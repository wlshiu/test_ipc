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
static uint32_t                 g_rpmsg_ready = 0;

static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;

//=============================================================================
//                  Private Function Definition
//=============================================================================
static int
_enable_interrupt(
    struct proc_vring   *vring_hw)
{
    return 0;
}

static void
_notify(
    int                 cpu_id,
    struct proc_intr    *intr_info)
{
    /* Trigger IPI (inter-processor interrupt) */
    core_attr_t     *pAttr_core = (core_attr_t*)intr_info->data;

    pthread_cond_signal(pAttr_core->pCores_irq_cond);
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
        err("%s", "rpmsg_send failed\n");
    }

    g_rpmsg_ready = 1;
#endif
}

static void
_rpmsg_channel_created(
    struct rpmsg_channel    *rp_chnl)
{
    (void)rp_chnl;
}

static void
_rpmsg_channel_deleted(
    struct rpmsg_channel    *rp_chnl)
{
    (void)rp_chnl;

}


static void
_isr_core_remote(void)
{
    pthread_cond_signal(&g_cond_rpmsg_rx);
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
        pthread_mutex_lock(&g_mtx_rpmsg_rx);
        pthread_cond_wait(&g_cond_rpmsg_rx, &g_mtx_rpmsg_rx);

        if( !g_rpmsg_ready )
            continue;

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

