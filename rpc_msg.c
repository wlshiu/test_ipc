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
    rpcq_t      hRpcQ[RPC_MSG_ID_TOTAL];

} rpc_msg_share_mem_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
rpc_msg_share_mem_t      g_share_buf;


static rpcq_t       *g_pRpcq = (rpcq_t*)&g_share_buf;
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void
_rpc_send_signal(void)
{
    return;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
rpc_msg_state_t
rpc_msg_init(
    rpc_msg_id_t  msg_id)
{
    rpc_msg_state_t     rval = RPC_MSG_STATE_OK;
    do {
        rpcq_t      *pRpcq_cur = &g_pRpcq[msg_id];

        if( msg_id >= RPC_MSG_ID_TOTAL )
        {
            rval = RPC_MSG_STATE_ERR_PARAM_INVALID;
            break;
        }

        RBUF_INIT(&pRpcq_cur->rpcq_hdr, RPC_MSG_MAX_QUEUE_SIZE, pRpcq_cur->buf_pool, RPCMSG_RBUF_ELEM_TYPE);
        pRpcq_cur->valid = RPCMSG_RBUF_VALID_NUMBER;

    } while(0);

    return rval;
}

rpc_msg_state_t
rpc_msg_deinit(
    rpc_msg_id_t  msg_id)
{
    rpc_msg_state_t     rval = RPC_MSG_STATE_OK;
    return rval;
}

rpc_msg_state_t
rpc_msg_push(
    rpc_msg_id_t    msg_id,
    void            *pData,
    uint32_t        timeout)
{
    rpc_msg_state_t     rval = RPC_MSG_STATE_OK;
    do {
        rpcq_t      *pRpcq_cur = &g_pRpcq[msg_id];

        if( msg_id >= RPC_MSG_ID_TOTAL )
        {
            rval = RPC_MSG_STATE_ERR_PARAM_INVALID;
            break;
        }

        if( !IS_RBUF_VALID(pRpcq_cur) )
        {
            rval = RPC_MSG_STATE_ERR_NO_INIT;
            break;
        }

        if( timeout == 0 && RBUF_IS_FULL(&pRpcq_cur->rpcq_hdr) )
        {
            rval = RPC_MSG_STATE_Q_FULL;
            break;
        }

        while( RBUF_IS_FULL(&pRpcq_cur->rpcq_hdr) )
        {
            if( timeout-- == 0 )
            {
                rval = RPC_MSG_STATE_ERR_TIMEOUT;
                break;
            }
        }

        if( rval )  break;

        RBUF_PUSH(&pRpcq_cur->rpcq_hdr, (RPCMSG_RBUF_ELEM_TYPE)pData);
        _rpc_send_signal();

    } while(0);
    return rval;
}


rpc_msg_state_t
rpc_msg_pop(
    rpc_msg_id_t    msg_id,
    void            **ppData,
    uint32_t        timeout)
{
    rpc_msg_state_t     rval = RPC_MSG_STATE_OK;
    do {
        rpcq_t      *pRpcq_cur = &g_pRpcq[msg_id];

        if( msg_id >= RPC_MSG_ID_TOTAL )
        {
            rval = RPC_MSG_STATE_ERR_PARAM_INVALID;
            break;
        }

        if( !IS_RBUF_VALID(pRpcq_cur) )
        {
            rval = RPC_MSG_STATE_ERR_NO_INIT;
            break;
        }

        if( timeout == 0 && RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr) )
        {
            rval = RPC_MSG_STATE_Q_EMPTY;
            break;
        }

        while( RBUF_IS_EMPTY(&pRpcq_cur->rpcq_hdr) )
        {
            if( timeout-- == 0 )
            {
                rval = RPC_MSG_STATE_ERR_TIMEOUT;
                break;
            }
        }

        RBUF_POP(&pRpcq_cur->rpcq_hdr, *(RPCMSG_RBUF_ELEM_TYPE*)ppData);

    } while(0);
    return rval;
}


rpc_msg_state_t
rpc_msg_notify(void)
{
    rpc_msg_state_t     rval = RPC_MSG_STATE_OK;
    _rpc_send_signal();
    return rval;
}

