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
/** @file rpc_binder.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU General Public License version 3
 * @description
 */

#ifndef __rpc_binder_H_wYV1VKNJ_lCQ5_H8Qw_szmX_uNwDnLZTKLdO__
#define __rpc_binder_H_wYV1VKNJ_lCQ5_H8Qw_szmX_uNwDnLZTKLdO__

#ifdef __cplusplus
extern "C" {
#endif

#include "rpc_err.h"
#include "rpc_msg.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define RPC_BINDER_MSG_NUM_MAX          RPC_MSG_MAX_QUEUE_SIZE

typedef enum rpc_binder_err
{
    RPC_BINDER_ERR_OK       = 0,
    RPC_BINDER_ERR_INVELID_PARAM,
    RPC_BINDER_ERR_UNKNOWN
} rpc_binder_err_t;

typedef enum rpc_binder_procedure_id
{
    RPC_BINDER_PROCEDURE_00     = 0,
    RPC_BINDER_PROCEDURE_01,
    RPC_BINDER_PROCEDURE_02,
    RPC_BINDER_PROCEDURE_03,
    RPC_BINDER_PROCEDURE_04,
    RPC_BINDER_PROCEDURE_05,
    RPC_BINDER_PROCEDURE_06,
    RPC_BINDER_PROCEDURE_07,
    RPC_BINDER_PROCEDURE_08,
    RPC_BINDER_PROCEDURE_09,
    RPC_BINDER_PROCEDURE_10,
    RPC_BINDER_PROCEDURE_11,
    RPC_BINDER_PROCEDURE_12,
    RPC_BINDER_PROCEDURE_13,
    RPC_BINDER_PROCEDURE_14,
    RPC_BINDER_PROCEDURE_15,

    RPC_BINDER_PROCEDURE_TOTAL
} rpc_binder_procedure_id_t;


//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct rpc_msg_box
{
    rpc_binder_procedure_id_t       procedurse_id;

    union {
        struct {
            uint32_t        core_id;
            uint32_t        sequence_num;
        } debug;

        struct {
            uint32_t    param[9];
        } def;
    } data;
} rpc_msg_box_t;

typedef void (*CB_RPC_EVENT_PROCEDURE)(rpc_msg_box_t *pBox, void *pTunnel_info);
//=============================================================================
//                  Global Data Definition
//=============================================================================

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
    uint32_t            buf_pool_len);


rpc_state_t
rpc_binder_recv(
    rpc_msg_channel_t   channel_id);


/**
 *  \brief rpc_binder_send()
 *
 *  \param [in] type        enum rpc_binder_type_t
 *  \param [in] pMsg_box    message box, it MUST be static buffer pointer
 *  \return Return          rpc_state_t
 *
 *  \details
 */
rpc_state_t
rpc_binder_send(
    rpc_msg_channel_t   channel_id,
    rpc_msg_box_t       *pMsg_box);


rpc_state_t
rpc_binder_register_procedure(
    rpc_binder_procedure_id_t   procedure_id,
    CB_RPC_EVENT_PROCEDURE      procedure,
    void                        *pTunnel_info);


rpc_state_t
rpc_binder_unregister_procedures(
    rpc_binder_procedure_id_t   procedure_id);


rpc_state_t
rpc_binder_register_send_handler(
    rpc_msg_channel_t       channel_id,
    CB_SEND_EVENT_HANDLER   handler,
    void                    *pTunnel_info);


int
rpc_binder_is_full(
    rpc_msg_channel_t       channel_id);



#ifdef __cplusplus
}
#endif

#endif
