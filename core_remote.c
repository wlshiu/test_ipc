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
#include "openamp/remoteproc.h"
#include "openamp/hil.h"
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
static struct hil_proc          g_hil_proc = {0};
static struct remote_proc       *g_pRProc = 0;
static uint32_t                 g_rpmsg_ready = 0;

static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;

//=============================================================================
//                  Private Function Definition
//=============================================================================
static int
_initialize(
    struct hil_proc     *proc)
{
    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);
    return 0;
}

static void
_release(
    struct hil_proc     *proc)
{
    pthread_cond_destroy(&g_cond_rpmsg_rx);
    pthread_mutex_destroy(&g_mtx_rpmsg_rx);
    return;
}

static void
_notify(
    struct hil_proc     *proc,
    struct proc_intr    *intr_info)
{
    /* Trigger IPI (inter-processor interrupt) */
    core_attr_t     *pAttr_core = (core_attr_t*)intr_info->data;

    pthread_cond_signal(&pAttr_core->irq_cond);
    return;
}

static int
_poll(
    struct hil_proc     *proc,
    int                 nonblock)
{
    /* wait irq event */
    pthread_mutex_lock(&g_mtx_rpmsg_rx);
    pthread_cond_wait(&g_cond_rpmsg_rx, &g_mtx_rpmsg_rx);
    pthread_mutex_unlock(&g_mtx_rpmsg_rx);

    hil_notified(proc, (uint32_t)(-1));
    return 0;
}

static struct hil_platform_ops  g_core1_proc_ops =
{
	.enable_interrupt = 0,
	.notify           = _notify,
	.boot_cpu         = 0,
	.shutdown_cpu     = 0,
	.poll             = _poll,
	.alloc_shm        = 0,
	.release_shm      = 0,
	.initialize       = _initialize,
	.release          = _release,
};

//---------------------------------------------
static void
_rpmsg_read_cb(
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

static struct remote_proc*
_remote_proc_open(
    core_attr_t     *pAttr_core)
{
    struct remote_proc      *pRProc = 0;
    do {
        if( !(pRProc = malloc(sizeof(struct remote_proc))) )
        {
            err("malloc %d fail\n", sizeof(struct remote_proc));
        }

        memset(pRProc, 0x0, sizeof(struct remote_proc));

        pRProc->role              = RPROC_REMOTE;
        pRProc->proc              = pAttr_core->pProc_cfg;
        pRProc->channel_created   = _rpmsg_channel_created;
        pRProc->channel_destroyed = _rpmsg_channel_deleted;
        pRProc->default_cb        = _rpmsg_read_cb;

    } while(0);

    return pRProc;
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
    int                 status = 0;

    pthread_detach(pthread_self());

    // create handle of remote processor
    g_pRProc = _remote_proc_open(pAttr_core);

    // init rpmsg which link to master
    status = rpmsg_init(g_pRProc->proc,
                        &g_pRProc->rdev,
                        g_pRProc->channel_created,
                        g_pRProc->channel_destroyed,
                        g_pRProc->default_cb,
                        g_pRProc->role);
    while(1)
    {
        hil_poll(g_pRProc->proc, 0);

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
    pAttr_core->pProc_cfg->ops = &g_core1_proc_ops;
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

