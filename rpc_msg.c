/*
 * Copyright (c) 2018-, Wei-Lun Hsu. All Rights Reserved.
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 3 of the
 * License.
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.
 * See the GNU General Public License version 3 for more details.
 */
/** @file rpc_msg.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU General Public License version 3
 * @description
 */



#include "rbuf.h"
#include "rpc_msg.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
#define RPCMSG_RBUF_ELEM_TYPE       uint32_t

#define RPCMSG_RBUF_VALID_NUMBER    0x55AA5A55
//=============================================================================
//                  Macro Definition
//=============================================================================
#define IS_RBUF_VALID(pRpcq)        ((pRpcq)->valid == RPCMSG_RBUF_VALID_NUMBER)
//=============================================================================
//                  Structure Definition
//=============================================================================
RBUF_TYPEDEF(uint32_t, rpcq_hdr_t);

typedef struct rpcq
{
    rpcq_hdr_t              rpcq_hdr;
    RPCMSG_RBUF_ELEM_TYPE   buf_pool[RPC_MSG_MAX_QUEUE_SIZE];
    uint32_t                valid;
} rpcq_t;

typedef struct rpc_msg_share_mem
{
    rpcq_t      hRpcQ[RPC_MSG_CHANNEL_TOTAL];

} rpc_msg_share_mem_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
#if 1
    static rpc_msg_share_mem_t      g_share_buf;
    static rpcq_t                   *g_pRpcq = (rpcq_t*)&g_share_buf;
#else
    static rpcq_t                   *g_pRpcq = (rpcq_t*)0xAABBCCDD;
#endif
//=============================================================================
//                  Private Function Definition
//=============================================================================
/**
 *  \brief _rpc_send_signal
 *
 *  \return     error code
 *
 *  \details
 *      broadcast event to other cores or remote devices
 */
static int
_rpc_send_signal(void)
{
    #if 0
    // ARM trigger IRQ to other cores
    asm volatile ("dsb");
    asm volatile ("sev");
    #endif
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
rpc_state_t
rpc_msg_init(
    rpc_msg_channel_t   channel_id,
    int                 msg_count)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        int         queue_size = 0;
        rpcq_t      *pRpcq_cur = &g_pRpcq[channel_id];

        if( channel_id >= RPC_MSG_CHANNEL_TOTAL )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        queue_size = (msg_count < RPC_MSG_MAX_QUEUE_SIZE) ? msg_count : RPC_MSG_MAX_QUEUE_SIZE;

        RBUF_INIT(&pRpcq_cur->rpcq_hdr, queue_size - 1, pRpcq_cur->buf_pool, RPCMSG_RBUF_ELEM_TYPE);
        pRpcq_cur->valid = RPCMSG_RBUF_VALID_NUMBER;

    } while(0);

    return rval;
}

rpc_state_t
rpc_msg_deinit(
    rpc_msg_channel_t  channel_id)
{
    rpc_state_t     rval = RPC_STATE_OK;
    return rval;
}

rpc_state_t
rpc_msg_push(
    rpc_msg_channel_t   channel_id,
    void                *pData,
    uint32_t            timeout)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        rpcq_t      *pRpcq_cur = &g_pRpcq[channel_id];

        if( channel_id >= RPC_MSG_CHANNEL_TOTAL )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        if( !IS_RBUF_VALID(pRpcq_cur) )
        {
            rval = RPC_STATE_ERR_NO_INIT;
            break;
        }

        if( timeout == 0 && RBUF_IS_FULL(&pRpcq_cur->rpcq_hdr) )
        {
            rval = RPC_STATE_Q_FULL;
            break;
        }

        while( RBUF_IS_FULL(&pRpcq_cur->rpcq_hdr) )
        {
            if( timeout-- == 0 )
            {
                rval = RPC_STATE_ERR_TIMEOUT;
                break;
            }
        }

        if( rval )  break;

        #if 1
        RBUF_PUSH(&pRpcq_cur->rpcq_hdr, (RPCMSG_RBUF_ELEM_TYPE)pData);
        #else
        do{
            (&pRpcq_cur->rpcq_hdr)->elems[(&pRpcq_cur->rpcq_hdr)->end] = (RPCMSG_RBUF_ELEM_TYPE)pData;
            (&pRpcq_cur->rpcq_hdr)->end = RBUF_NEXT_END_IDX(&pRpcq_cur->rpcq_hdr);
            if (RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr)) {
                (&pRpcq_cur->rpcq_hdr)->start = RBUF_NEXT_START_IDX(&pRpcq_cur->rpcq_hdr);
            }
        }while(0);
        #endif

        _rpc_send_signal();

    } while(0);
    return rval;
}


rpc_state_t
rpc_msg_pop(
    rpc_msg_channel_t   channel_id,
    void                **ppData,
    uint32_t            timeout)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        rpcq_t      *pRpcq_cur = &g_pRpcq[channel_id];

        if( channel_id >= RPC_MSG_CHANNEL_TOTAL )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        if( !IS_RBUF_VALID(pRpcq_cur) )
        {
            rval = RPC_STATE_ERR_NO_INIT;
            break;
        }

        if( timeout == 0 && RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr) )
        {
            rval = RPC_STATE_Q_EMPTY;
            break;
        }

        while( RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr) )
        {
            if( timeout-- == 0 )
            {
                rval = RPC_STATE_ERR_TIMEOUT;
                break;
            }
        }

        #if 1
        RBUF_POP(&pRpcq_cur->rpcq_hdr, *(RPCMSG_RBUF_ELEM_TYPE*)ppData);
        #else
        do{
            if (RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr))
                break;

            *(RPCMSG_RBUF_ELEM_TYPE*)ppData = (&pRpcq_cur->rpcq_hdr)->elems[(&pRpcq_cur->rpcq_hdr)->start];
            (&pRpcq_cur->rpcq_hdr)->start = RBUF_NEXT_START_IDX(&pRpcq_cur->rpcq_hdr);
        }while(0);
        #endif

    } while(0);
    return rval;
}


rpc_state_t
rpc_msg_notify(void)
{
    rpc_state_t     rval = RPC_STATE_OK;
    _rpc_send_signal();
    return rval;
}

