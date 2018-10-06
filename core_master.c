/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file core_master.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/03
 * @license
 * @description
 */


#include <windows.h>
#include "core_master.h"
#include "core_remote.h"
#include "rpmsg/rpmsg.h"
#include "common/hil/hil.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct remote_table
{
    core_attr_t     *pAttr[CORE_ID_TOTAL];
    uint32_t        core_cnt;

} remote_table_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static remote_table_t           g_remote_table = {0};
static struct rpmsg_channel     *g_act_chnnl = 0;
static uint32_t                 g_rpmsg_ready = 0;

static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;


//=============================================================================
//                  Private Function Definition
//=============================================================================
#if 0
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

static struct hil_platform_ops      g_core0_proc_ops =
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
#endif

//---------------------------------------------
static void
_rpmsg_channel_created(
    struct rpmsg_channel    *rp_chnl)
{
    g_act_chnnl = rp_chnl;

//    rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
//                              RPMSG_ADDR_ANY);
}

static void
_rpmsg_channel_deleted(
    struct rpmsg_channel    *rp_chnl)
{
//    rpmsg_destroy_ept(rp_ept);
}

static void
_rpmsg_read_cb(
    struct rpmsg_channel    *rp_chnl,
    void                    *data,
    int                     len,
    void                    *priv,
    unsigned long           src)
{
#if 0
    if ((*(int *)data) == SHUTDOWN_MSG)
    {
        shutdown_called = 1;
    }
    else
    {
        /* Send data back to remote */
        rpmsg_send(rp_chnl, data, len);
    }
#else
    g_rpmsg_ready = 1;
#endif
}


static void
_isr_core_master(void)
{
    pthread_cond_signal(&g_cond_rpmsg_rx);
    return;
}

static void*
_task_core_master(void *argv)
{
    remote_table_t      *pRPoc_table = (remote_table_t*)argv;

    pthread_detach(pthread_self());

#if 1
    // boot remote processor
    for(int i = CORE_ID_REMOTE_1; i < CORE_ID_TOTAL; ++i)
    {
        core_remote_boot(pRPoc_table->pAttr[i]);
    }
#endif // 0

    while(1)
    {
        if( !g_rpmsg_ready )
            continue;


        // send rpmsg

        Sleep((rand() >> 5)& 0x3);
    }
    pthread_exit(0);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int
core_master_init(
    core_attr_t     *pAttr_core)
{
    g_rpmsg_ready = 0;

    pAttr_core->pf_irs = _isr_core_master;
    return 0;
}

int
core_master_boot(
    core_attr_t     *pAttr,
    uint32_t        core_num)
{
    do {
        pthread_t       t;

        g_remote_table.core_cnt = (core_num > CORE_ID_TOTAL) ? CORE_ID_TOTAL : core_num;
        for(int i = 0; i < g_remote_table.core_cnt; ++i)
        {
            g_remote_table.pAttr[i] = pAttr;
        }

        pthread_create(&t, 0, _task_core_master, &g_remote_table);
    } while(0);
    return 0;
}

