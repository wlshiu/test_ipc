/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file share_info.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/19
 * @license
 * @description
 */

#ifndef __share_info_H_wuVuuNQt_lzJp_HjuT_sty6_uSe96mud7vom__
#define __share_info_H_wuVuuNQt_lzJp_HjuT_sty6_uSe96mud7vom__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "platform/rpmsg_platform.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
//=============================================================================
#define RPMSG_LITE_CHANNEL_0                RL_PLATFORM_M4_M0_LINK_ID
#define RPMSG_LITE_CHANNEL_1                RL_PLATFORM_M3_M0_LINK_ID

#define EPT_ADDR_0                      (30)
#define EPT_ADDR_1                      (40)
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
extern uint32_t         g_share_mem[500 << 10];

#define RPMSG_LITE_SHMEM_BASE       (&g_share_mem)
#define RPMSG_LITE_SHMEM_SIZE       sizeof(g_share_mem)
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
