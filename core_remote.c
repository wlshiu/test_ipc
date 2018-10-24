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

#include "rpmsg_lite.h"
#include "rpmsg_ns.h"
#include "rpmsg_platform.h"

#include "share_info.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define LOCAL_EPT_ADDR                      EPT_ADDR_0
#define REMOTE_EPT_ADDR                     EPT_ADDR_1

#define RPMSG_LITE_NS_ANNOUNCE_STRING       "hollow"
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

#ifdef msg
#undef msg
#endif // msg
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
static pthread_cond_t           g_cond_rpmsg_rx;
static pthread_mutex_t          g_mtx_rpmsg_rx;

static msg_box_t                g_msgbox = {0};

static unsigned long            g_remote_addr = 0lu;

extern core_attr_t         g_core_attr[CORE_ID_TOTAL];
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void
_remote_trigger_irq(
    core_attr_t     *pAttr)
{
    static int     core_event = 0;
    queue_push(pAttr->pRemote_irq_q, core_event++);
    return;
}

static void
_remote_notify(int vector_id)
{
    core_attr_t     *pAttr_cur = &g_core_attr[CORE_ID_REMOTE_1];

    trace_enter();

    switch( RL_GET_LINK_ID(vector_id) )
    {
        case RPMSG_LITE_CHANNEL_0:
            vector_id &= (RL_MSK_LINK_ID | RL_MSK_Q_ID);

            queue_push(pAttr_cur->pVring_tx_q, vector_id);

            _remote_trigger_irq(pAttr_cur);
            break;
        default:    break;
    }

    return;
}

static platform_ops_t   g_remote_platform_ops =
{
    .notify = _remote_notify,
};
//--------------------------------------
static int  g_irq_remote_cnt = 0;
static void
_isr_core_remote(void)
{
    int             rval = 0;
    core_attr_t     *pAttr_cur = &g_core_attr[CORE_ID_REMOTE_1];
    uint32_t        vect_id = (uint32_t)-1;

    g_irq_remote_cnt++;
    trace_enter();

#if 1
    if( (rval = queue_pop(pAttr_cur->pVring_rx_q, (int*)&vect_id)) )
        return;
#else
    while( !(rval = queue_pop(pAttr_cur->pVring_rx_q, (int*)&vect_id)) )
#endif // 1

    {
        vect_id |= (CORE_ID_REMOTE_1 << RL_SHFT_CORE_ID);

        if( RL_GET_Q_ID(vect_id) < VRING_ISR_COUNT )
        {
            platform_rpmsg_handler(vect_id);
        }
    }

    return;
}

static int
_cb_remote_ept_read(
    void            *payload,
    int             payload_len,
    unsigned long   src,
    void            *priv)
{
    int     *pHas_received = priv;

    trace_enter();

    if( payload_len <= sizeof(msg_box_t) )
    {
//        msg_box_t   msgbox = {0};

        memcpy((void *)&g_msgbox, payload, payload_len);
        g_remote_addr = src;

        printf("[Remote recv] Msg: ------------DATA = %d\n", g_msgbox.data++);

        pthread_mutex_lock(&g_mtx_rpmsg_rx);
        *pHas_received = 1;
        pthread_mutex_unlock(&g_mtx_rpmsg_rx);
    }
    return RL_RELEASE;
}

static void*
_task_core_remote(void *argv)
{
    volatile int                           has_received = 0;
    struct rpmsg_lite_ept_static_context   my_ept_context;
    struct rpmsg_lite_endpoint             *my_ept;
    struct rpmsg_lite_instance             rpmsg_ctxt;
    struct rpmsg_lite_instance             *my_rpmsg;

    pthread_detach(pthread_self());

    pthread_cond_init(&g_cond_rpmsg_rx, NULL);
    pthread_mutex_init(&g_mtx_rpmsg_rx, NULL);

    platform_init(CORE_ID_REMOTE_1, &g_remote_platform_ops);

    my_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE,
                                      CORE_ID_REMOTE_1,
                                      RPMSG_LITE_CHANNEL_0,
                                      RL_NO_FLAGS,
                                      &rpmsg_ctxt);

    msg("%s", "remote init done\n");
    while (!rpmsg_lite_is_link_up(my_rpmsg))
    {
        Sleep(1); // wait
    }

    pthread_mutex_lock(&g_mtx_rpmsg_rx);
    has_received = 0;
    pthread_mutex_unlock(&g_mtx_rpmsg_rx);

    my_ept = rpmsg_lite_create_ept(my_rpmsg,
                                   LOCAL_EPT_ADDR,
                                   _cb_remote_ept_read,
                                   (void *)&has_received,
                                   &my_ept_context);

    msg("created ept addr = %d done\n", LOCAL_EPT_ADDR);

#ifdef RPMSG_LITE_NS_USED
    rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);
#endif /*RPMSG_LITE_NS_USED*/

    while(1)
    {
    #if 1
        if( has_received )
        {
//            static msg_box_t       msgbox = {0};

            pthread_mutex_lock(&g_mtx_rpmsg_rx);
            has_received = 0;
            pthread_mutex_unlock(&g_mtx_rpmsg_rx);

            rpmsg_lite_send(my_rpmsg, my_ept,
                            g_remote_addr,
                            (char *)&g_msgbox,
                            sizeof(msg_box_t),
                            RL_DONT_BLOCK);

            msg("sent (to addr=%ld) msg done\n", g_remote_addr);
        }
    #endif // 0

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
core_remote_init(
    core_attr_t     *pAttr_core)
{
    g_remote_addr = 0;
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

