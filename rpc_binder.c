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
/** @file rpc_binder.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU General Public License version 3
 * @description
 */


#include "rpc_binder.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct rpc_procedure_node
{
    CB_RPC_EVENT_PROCEDURE      cb_usr_procedures;
    void                        *pTunnel_info;
} rpc_procedure_node_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static rpc_procedure_node_t       g_rpc_usr_procedures[RPC_BINDER_PROCEDURE_TOTAL] = {0};
//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
rpc_state_t
rpc_binder_init(
    rpc_msg_channel_t   channel_id,
    uint32_t            *pBuf_pool,
    uint32_t            buf_pool_len)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        int     count = 0;

        count = buf_pool_len / sizeof(rpc_msg_box_t);
        if( count <= 1 )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        rpc_msg_init(channel_id, count);

    } while(0);
    return rval;
}

rpc_state_t
rpc_binder_deinit()
{
    rpc_state_t     rval = RPC_STATE_OK;
    return rval;
}

rpc_state_t
rpc_binder_recv(
    rpc_msg_channel_t   channel_id)
{
    rpc_state_t     rval = RPC_STATE_OK;

    do {
        rpc_msg_box_t        *pMsg_box = 0;
        uint32_t             timeout = 0;

        rval = rpc_msg_pop(channel_id, (void**)&pMsg_box, timeout);
        if( rval )     break;

        if( pMsg_box && pMsg_box->procedurse_id < RPC_BINDER_PROCEDURE_TOTAL )
        {
            rpc_procedure_node_t        *pCur_node = &g_rpc_usr_procedures[pMsg_box->procedurse_id];
            printf("%d, %d, x%x\n", pMsg_box->procedurse_id, pMsg_box->data.def.param[0], pMsg_box->data.def.param[1]);

            if( pCur_node->cb_usr_procedures )
                pCur_node->cb_usr_procedures(pMsg_box, pCur_node->pTunnel_info);
        }
    } while(0);

    return rval;
}

/**
 *  \brief rpc_binder_send()
 *
 *  \param [in] channel_id  enum rpc_msg_channel_t
 *  \param [in] pMsg_box    message box, it MUST be static buffer pointer
 *  \return Return          rpc_state_t
 *
 *  \details
 */
rpc_state_t
rpc_binder_send(
    rpc_msg_channel_t   channel_id,
    rpc_msg_box_t       *pMsg_box)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        uint32_t             timeout = 0;

        rval = rpc_msg_push(channel_id, pMsg_box, timeout);
        if( rval )     break;

    } while(0);

    return rval;
}

/**
 *  \brief rpc_binder_procedure_register()
 *
 *  \param [in] id              enum rpc_binder_event_id_t
 *  \param [in] procedure       callback function if be triggered
 *  \param [in] pTunnel_info    some info which you want through to callback function
 *  \return Return              through
 *
 *  \details
 */
rpc_state_t
rpc_binder_procedure_register(
    rpc_binder_procedure_id_t   procedure_id,
    CB_RPC_EVENT_PROCEDURE      procedure,
    void                        *pTunnel_info)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        if( procedure_id >= RPC_BINDER_PROCEDURE_TOTAL )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        g_rpc_usr_procedures[procedure_id].cb_usr_procedures = procedure;
        g_rpc_usr_procedures[procedure_id].pTunnel_info      = pTunnel_info;
    } while(0);

    return rval;
}


rpc_state_t
rpc_binder_procedure_unregister(
    rpc_binder_procedure_id_t   procedure_id)
{
    rpc_state_t     rval = RPC_STATE_OK;
    do {
        if( procedure_id >= RPC_BINDER_PROCEDURE_TOTAL )
        {
            rval = RPC_STATE_ERR_INVELID_PARAM;
            break;
        }

        g_rpc_usr_procedures[procedure_id].cb_usr_procedures = 0;
        g_rpc_usr_procedures[procedure_id].pTunnel_info      = 0;
    } while(0);

    return rval;
}
