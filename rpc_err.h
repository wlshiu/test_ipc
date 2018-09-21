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
/** @file rpc_err.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/09/20
 * @license GNU General Public License version 3
 * @description
 */

#ifndef __rpc_err_H_wS2gOHUt_leyG_HiPl_sEyu_uHSTCt2l8YKC__
#define __rpc_err_H_wS2gOHUt_leyG_HiPl_sEyu_uHSTCt2l8YKC__

#ifdef __cplusplus
extern "C" {
#endif


//=============================================================================
//                  Constant Definition
//=============================================================================
typedef enum rpc_state
{
    RPC_STATE_Q_EMPTY               = 2,
    RPC_STATE_Q_FULL                = 1,
    RPC_STATE_OK                    = 0,
    RPC_STATE_ERR_NO_INIT           = -1,
    RPC_STATE_ERR_TIMEOUT           = -2,
    RPC_STATE_ERR_INVELID_PARAM     = -3,
    RPC_STATE_ERR_UNKNOWN           = -4,
} rpc_state_t;
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

#ifdef __cplusplus
}
#endif

#endif
