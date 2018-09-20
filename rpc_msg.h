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
/** @file rpc_msg.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU GENERAL PUBLIC LICENSE Version 3
 * @description
 */

#ifndef __rpc_msg_H_wVNzyewp_lXGj_Hg74_sKFm_uvInHKzfNGgA__
#define __rpc_msg_H_wVNzyewp_lXGj_Hg74_sKFm_uvInHKzfNGgA__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//=============================================================================
//                  Constant Definition
//=============================================================================
#define RPC_MSG_MAX_QUEUE_SIZE      2


typedef enum rpc_msg_state
{
    RPC_MSG_STATE_OK        = 0,
    RPC_MSG_STATE_Q_EMPTY,
    RPC_MSG_STATE_Q_FULL,
    RPC_MSG_STATE_ERR_PARAM_INVALID,
    RPC_MSG_STATE_ERR_NO_INIT,
    RPC_MSG_STATE_ERR_TIMEOUT,
    RPC_MSG_STATE_ERR_UNKNOWN,

} rpc_msg_state_t;


typedef enum rpc_msg_id
{
    RPC_MSG_ID_CORE0_CORE1,
    RPC_MSG_ID_CORE1_CORE0,

    RPC_MSG_ID_TOTAL
} rpc_msg_id_t;


//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
rpc_msg_state_t
rpc_msg_init(
    rpc_msg_id_t  msg_id);


rpc_msg_state_t
rpc_msg_push(
    rpc_msg_id_t    msg_id,
    void            *pData,
    uint32_t        timeout);


rpc_msg_state_t
rpc_msg_pop(
    rpc_msg_id_t    msg_id,
    void            **ppData,
    uint32_t        timeout);


rpc_msg_state_t
rpc_msg_notify(void);


#ifdef __cplusplus
}
#endif

#endif
