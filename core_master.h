/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file core_master.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/03
 * @license
 * @description
 */

#ifndef __core_master_H_wrLswoUt_lPrO_Hmer_si9W_uF4NsBSlWBBc__
#define __core_master_H_wrLswoUt_lPrO_Hmer_si9W_uF4NsBSlWBBc__

#ifdef __cplusplus
extern "C" {
#endif


#include "core_info.h"
//=============================================================================
//                  Constant Definition
//=============================================================================

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
int
core_master_init(
    core_attr_t     *pAttr_core);


int
core_master_boot(
    core_attr_t     *pAttr,
    uint32_t        core_num);


#ifdef __cplusplus
}
#endif

#endif
