/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file core_info.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/01
 * @license
 * @description
 */

#ifndef __core_info_H_wBZxsyBn_lOzr_H6P8_s8PK_uEN0hHD2DlBM__
#define __core_info_H_wBZxsyBn_lOzr_H6P8_s8PK_uEN0hHD2DlBM__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <pthread.h>
//=============================================================================
//                  Constant Definition
//=============================================================================
typedef enum core_id
{
    CORE_ID_0       = 0,
    CORE_ID_1,
    CORE_ID_TOTAL,
} core_id_t;
//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef void (*cb_irs)(void);

typedef struct core_attr
{
    core_id_t           core_id;
    pthread_cond_t      cond;
    cb_irs              pf_irs;
} core_attr_t;
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
core_irq_simulator(
    core_attr_t     *pAttr,
    uint32_t        core_num);



#ifdef __cplusplus
}
#endif

#endif
