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

#include "irq_queue.h"

#include "rpmsg_lite.h"
#include "platform/rpmsg_platform.h"

#include "share_info.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define LOCAL_EPT_ADDR                      EPT_ADDR_1
#define REMOTE_EPT_ADDR                     EPT_ADDR_0
//=============================================================================
//                  Macro Definition
//=============================================================================
#if 1
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

static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;

extern core_attr_t         g_core_attr[CORE_ID_TOTAL];
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void
_master_trigger_irq(
    core_attr_t     *pAttr)
{
    static int     core_event = 0;
    queue_push(pAttr->pRemote_irq_q, core_event++);
    return;
}

static void
_master_notify(int vector_id)
{
    core_attr_t     *pAttr_cur = &g_core_attr[CORE_ID_MASTER];

    trace_enter();

    switch( RL_GET_LINK_ID(vector_id) )
    {
        case RPMSG_LITE_CHANNEL_0:
            queue_push(pAttr_cur->pVring_tx_q, vector_id);

            _master_trigger_irq(pAttr_cur);
            break;
        default:    break;
    }

    return;
}

static platform_ops_t   g_master_platform_ops =
{
    .notify = _master_notify,
};
//--------------------------------------

static void
_isr_core_master(void)
{
    core_attr_t     *pAttr_cur = &g_core_attr[CORE_ID_MASTER];
    uint32_t        vect_id = (uint32_t)-1;

    trace_enter();
    queue_pop(pAttr_cur->pVring_rx_q, (int*)&vect_id);

    if( vect_id < VRING_ISR_COUNT )
    {
        platform_rpmsg_handler(vect_id);
    }
    return;
}

static int
_cb_master_ept_read(
    void            *payload,
    int             payload_len,
    unsigned long   src,
    void            *priv)
{
    int     *pHas_received = priv;

    trace_enter();

    if (payload_len <= sizeof(msg_box_t))
    {
        msg_box_t   msg = {0};
        memcpy((void *)&msg, payload, payload_len);
        *pHas_received = 1;

        msg.data++;

        printf("Primary core received a msg\n\r");
    // printf("Message: Size=%x, DATA = %i\n\r", payload_len, msg.data);
    }

    return RL_RELEASE;
}

static void*
_task_core_master(void *argv)
{
    remote_table_t      *pRPoc_table = (remote_table_t*)argv;
    // core_attr_t         *pAttr = pRPoc_table->pAttr[CORE_ID_MASTER];

    volatile int                            has_received = 0;
    rpmsg_lite_ept_static_context_t         my_ept_context;
    rpmsg_lite_endpoint_t                   *my_ept;
    rpmsg_lite_instance_t                   rpmsg_ctxt;
    rpmsg_lite_instance_t                   *my_rpmsg;
    msg_box_t                               msg = {0};

    pthread_detach(pthread_self());

    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);

    platform_init(&g_master_platform_ops);

#if 1
    // boot remote processor
    for(int i = CORE_ID_REMOTE_1; i < CORE_ID_TOTAL; ++i)
    {
        core_remote_boot(pRPoc_table->pAttr[i]);
    }
#endif // 0

    my_rpmsg = rpmsg_lite_master_init(RPMSG_LITE_SHMEM_BASE,
                                      RPMSG_LITE_SHMEM_SIZE,
                                      RPMSG_LITE_CHANNEL_0,
                                      RL_NO_FLAGS,
                                      &rpmsg_ctxt);

    my_ept = rpmsg_lite_create_ept(my_rpmsg,
                                   LOCAL_EPT_ADDR,
                                   _cb_master_ept_read,
                                   (void *)&has_received,
                                   &my_ept_context);

#if 1
    /* Send the first message to the remoteproc */
    msg.data = 10;
    rpmsg_lite_send(my_rpmsg, my_ept,
                    REMOTE_EPT_ADDR,
                    (char *)&msg,
                    sizeof(msg_box_t),
                    RL_DONT_BLOCK);
#endif // 1

    while(1)
    {
    #if 0
        if( has_received )
        {
            has_received = 0;
            msg.data++;
            rpmsg_lite_send(my_rpmsg, my_ept,
                            REMOTE_EPT_ADDR,
                            (char *)&msg,
                            sizeof(msg_box_t),
                            RL_DONT_BLOCK);
        }
    #endif

        Sleep((rand() >> 5)& 0x3);
    }

    rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
    my_ept = NULL;

    rpmsg_lite_deinit(my_rpmsg);
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

