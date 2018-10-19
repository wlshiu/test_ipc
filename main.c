/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file main.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/19
 * @license
 * @description
 */

#include <stdint.h>
#include <string.h>
#include "rpmsg_lite.h"
#include "platform/rpmsg_platform.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define RPMSG_LITE_LINK_ID                  (RL_PLATFORM_LPC5411x_M4_M0_LINK_ID)
#define LOCAL_EPT_ADDR                      (30)
#define REMOTE_EPT_ADDR                     (40)
//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct msg_box
{
    uint32_t    data;
} msg_box_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static uint32_t         g_share_mem[500 << 10] = {0};

#define RPMSG_LITE_SHMEM_BASE       (&g_share_mem)
#define RPMSG_LITE_SHMEM_SIZE       sizeof(g_share_mem)


static unsigned long        g_remote_addr = 0lu;
//=============================================================================
//                  Private Function Definition
//=============================================================================
static int
_cb_remote_ept_read(
    void            *payload,
    int             payload_len,
    unsigned long   src,
    void            *priv)
{
    int     *pHas_received = priv;

    if (payload_len <= sizeof(msg_box_t))
    {
        msg_box_t   msg = {0};

        memcpy((void *)&msg, payload, payload_len);
        g_remote_addr = src;
        *pHas_received = 1;
//        printf("The m0+ core received msg from m4 core\r\n");
//        printf("Message: Size=%x, DATA = %i\n\r", payload_len, msg.DATA);
    }
    return RL_RELEASE;
}

static void
_core_remote()
{
    char                                        ch = 0;
    volatile int                                has_received = 0;
    struct rpmsg_lite_ept_static_context        my_ept_context;
    struct rpmsg_lite_endpoint                  *my_ept;
    struct rpmsg_lite_instance                  rpmsg_ctxt;
    struct rpmsg_lite_instance                  *my_rpmsg;

    my_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE,
                                      RPMSG_LITE_LINK_ID,
                                      RL_NO_FLAGS,
                                      &rpmsg_ctxt);

    while (!rpmsg_lite_is_link_up(my_rpmsg))
        ;

    my_ept = rpmsg_lite_create_ept(my_rpmsg,
                                   LOCAL_EPT_ADDR,
                                   _cb_remote_ept_read,
                                   (void *)&has_received,
                                   &my_ept_context);

#ifdef RPMSG_LITE_NS_USED
    rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);
#endif /*RPMSG_LITE_NS_USED*/

    msg_box_t       msg = {0};

    has_received = 0;
    //msg.data=ch;
    while (1) //rx received data
    {
//        ch = GETCHAR(); // get date from callback
        msg.data = ch - 49;

        if (msg.data)
        {
            has_received = 0;
            // msg.DATA++;
            rpmsg_lite_send(my_rpmsg, my_ept,
                            g_remote_addr,
                            (char *)&msg, sizeof(msg_box_t),
                            RL_DONT_BLOCK);
        }
    }

    rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
    my_ept = NULL;
    rpmsg_lite_deinit(my_rpmsg);
    msg.data = 0;
    return;
}


static int
_cb_master_ept_read(
    void            *payload,
    int             payload_len,
    unsigned long   src,
    void            *priv)
{
    int *pHas_received = priv;

    if (payload_len <= sizeof(msg_box_t))
    {
        msg_box_t   msg = {0};
        memcpy((void *)&msg, payload, payload_len);
        *pHas_received = 1;

        msg.data++;
    // printf("Primary core received a msg\n\r");
    // printf("Message: Size=%x, DATA = %i\n\r", payload_len, msg.data);
    }

    return RL_RELEASE;
}

static void
_core_master()
{
    volatile int                            has_received;
    struct rpmsg_lite_ept_static_context    my_ept_context;
    struct rpmsg_lite_endpoint              *my_ept;
    struct rpmsg_lite_instance              rpmsg_ctxt;
    struct rpmsg_lite_instance              *my_rpmsg;

    my_rpmsg = rpmsg_lite_master_init(RPMSG_LITE_SHMEM_BASE,
                                      RPMSG_LITE_SHMEM_SIZE,
                                      RPMSG_LITE_LINK_ID,
                                      RL_NO_FLAGS,
                                      &rpmsg_ctxt);

    my_ept = rpmsg_lite_create_ept(my_rpmsg,
                                   LOCAL_EPT_ADDR,
                                   _cb_master_ept_read,
                                   (void *)&has_received,
                                   &my_ept_context);

    has_received = 0;

    msg_box_t       msg = {0};

    /* Send the first message to the remoteproc */
    msg.data = 10;
    rpmsg_lite_send(my_rpmsg, my_ept,
                    REMOTE_EPT_ADDR,
                    (char *)&msg,
                    sizeof(msg_box_t),
                    RL_DONT_BLOCK);

    while (msg.data <= 100)
    {
        if (has_received)
        {
            has_received = 0;
            msg.data++;
            rpmsg_lite_send(my_rpmsg, my_ept,
                            REMOTE_EPT_ADDR,
                            (char *)&msg,
                            sizeof(msg_box_t),
                            RL_DONT_BLOCK);
        }
    }

    rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
    my_ept = NULL;
    rpmsg_lite_deinit(my_rpmsg);
    return;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int main()
{
    _core_master();
    return 0;
}
