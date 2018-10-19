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

#include "irq_queue.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
typedef enum core_id
{
    CORE_ID_MASTER       = 0,
    CORE_ID_REMOTE_1,
    CORE_ID_TOTAL,
} core_id_t;
//=============================================================================
//                  Macro Definition
//=============================================================================
#define err(str, argv...)           do{ printf("%s[%d] " str, __func__, __LINE__, ##argv); while(1);}while(0)


//=============================================================================
//                  Structure Definition
//=============================================================================
typedef void (*cb_irs)(void);

typedef struct core_attr
{
    core_id_t           core_id;

    queue_handle_t      irq_q;
    cb_irs              pf_irs;

    queue_handle_t      *pRemote_irq_q;

} core_attr_t;

/* Max supported ISR counts */
#define VRING_ISR_COUNT                 2

typedef struct vring_isr_info
{
    int     vector_id;
    void    *data;
    void    (*vring_isr)(int vector_id, void *data);

} vring_isr_info_t;
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
