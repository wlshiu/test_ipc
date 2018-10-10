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
#include "porting/platform_info.h"

#include "irq_queue.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define LOCAL_VRING_VECT_TX     VRING_VECT_0
#define LOCAL_VRING_VECT_RX     VRING_VECT_1
//=============================================================================
//                  Macro Definition
//=============================================================================
#if 0
#define trace_enter()                do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][master] enter\n", __func__, __LINE__); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)

#define trace_leave()                do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][master] leave\n", __func__, __LINE__); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)
#else
    #define trace_enter()
    #define trace_leave()
#endif


#define msg(str, argv...)           do{ extern pthread_mutex_t   g_log_mtx; \
                                        pthread_mutex_lock(&g_log_mtx); \
                                        printf("%s[%d][master] "str, __func__, __LINE__, ##argv); \
                                        pthread_mutex_unlock(&g_log_mtx); \
                                    }while(0)
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

static vring_isr_info_t         g_vring_isr[VRING_VECT_TOTAL] = {0};

extern queue_handle_t      g_vring_irq_q_0;
extern queue_handle_t      g_vring_irq_q_1;
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

        trace_enter();
        msg("vent_id= %d\n", intr_info->vect_id);
        queue_push(&g_vring_irq_q_0, intr_info->vect_id);

        {
            static int     event = 0;
//            pthread_cond_signal(pAttr_core->pCores_irq_cond);
            queue_push(pAttr_core->pRemote_irq_q, event++);
        }

        trace_leave();
    } while(0);

    return;
}


static struct hil_platform_ops      g_core0_proc_ops =
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
_rpmsg_channel_created(
    struct rpmsg_channel    *rp_chnl)
{
    g_act_chnnl = rp_chnl;

//    rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
//                              RPMSG_ADDR_ANY);

//    g_rpmsg_ready = 1;
    trace_enter();
//    msg("%s\n", "enter");
    return;
}

static void
_rpmsg_channel_deleted(
    struct rpmsg_channel    *rp_chnl)
{
//    rpmsg_destroy_ept(rp_ept);

    trace_enter();
    return;
}

static void
_rpmsg_recv_cb(
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

    trace_enter();
#endif

    return;
}

//static void
//_isr_core_remote(void)
//{
//    return;
//}

static void
_isr_core_master(void)
{
//    pthread_cond_signal(&g_cond_rpmsg_rx);

    uint32_t                        vect_id = (uint32_t)-1;

    trace_enter();
    queue_pop(&g_vring_irq_q_1, (int*)&vect_id);

    if( vect_id < VRING_ISR_COUNT )
    {
        vring_isr_info_t    *pInfo = &g_vring_isr[vect_id];

        if( pInfo->vring_isr )
            pInfo->vring_isr(vect_id, pInfo->data);

        trace_leave();
    }
    return;
}

static void*
_task_core_master(void *argv)
{
    remote_table_t      *pRPoc_table = (remote_table_t*)argv;
    core_attr_t         *pAttr = pRPoc_table->pAttr[CORE_ID_MASTER];
    remote_device_t     *pRDev = 0;

    pthread_detach(pthread_self());

    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);

    platform_set_ops(pAttr->core_id, &g_core0_proc_ops);
    platform_set_vring_intr_priv_data(pAttr->core_id, pAttr);

#if 1
    // boot remote processor
    for(int i = CORE_ID_REMOTE_1; i < CORE_ID_TOTAL; ++i)
    {
        core_remote_boot(pRPoc_table->pAttr[i]);
    }
#endif // 0

    rpmsg_init(RPMSG_REMOTE,
               &pRDev,
               _rpmsg_channel_created,
               _rpmsg_channel_deleted,
               _rpmsg_recv_cb,
               RPMSG_MASTER);

    while(1)
    {
        void                *pTX_buf = 0;
        unsigned long       buf_size = 0;

//        pthread_mutex_lock(&g_mtx_rpmsg_rx);
//        pthread_cond_wait(&g_cond_rpmsg_rx, &g_mtx_rpmsg_rx);

//        if( !g_rpmsg_ready )
//            continue;
#if 0
        pTX_buf = rpmsg_alloc_tx_buffer(g_act_chnnl, &buf_size, RPMSG_TRUE);
        if( !pTX_buf )      continue;

        memcpy(pTX_buf, app_buf, len);

        /* Echo back received message with nocopy send */
        rpmsg_sendto_nocopy(g_act_chnnl, pTX_buf, len, app_msg[app_idx].src);

        rpmsg_release_rx_buffer(g_act_chnnl, app_msg[app_idx].data)

        g_rpmsg_ready = 0;
#endif
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
            g_remote_table.pAttr[i] = pAttr++;
        }

        pthread_create(&t, 0, _task_core_master, &g_remote_table);
    } while(0);
    return 0;
}

