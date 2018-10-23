/*
 * The Clear BSD License
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "platform/rpmsg_platform.h"
#include "rpmsg_env.h"


//==========================================
//#include "core_info.h"
#include "irq_queue.h"
#include "share_info.h"

static platform_ops_t       *g_pPlatform_ops[4] = {0};

//extern core_attr_t         g_core_attr[CORE_ID_TOTAL];

extern queue_handle_t      g_vring_irq_q_0;
extern queue_handle_t      g_vring_irq_q_1;
//==========================================

int platform_init_interrupt(int vq_id, void *isr_data)
{
    /* Register ISR to environment layer */
    env_register_isr(vq_id, isr_data);
    return 0;
}

int platform_deinit_interrupt(int vq_id)
{
    env_unregister_isr(vq_id);
    return 0;
}

/**
 *  vq_id[31:1] = channel id
 *  vq_id[0]    = vring id
 */
void platform_notify(int vq_id)
{
    int             core_id = RL_GET_CORE_ID(vq_id);
    platform_ops_t  *pOps = 0;

    pOps = g_pPlatform_ops[core_id];
    if( pOps && pOps->notify )
        pOps->notify(vq_id);

    return;
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(int num_msec)
{
    env_sleep_msec(num_msec);
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int platform_in_isr(void)
{
    return 0;
}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt with passed priority and type.
 *
 * @param vq_id Vector ID that need to be converted to IRQ number
 * @param trigger_type IRQ active level
 * @param trigger_type IRQ priority
 *
 * @return vq_id. Return value is never checked..
 *
 */
int platform_interrupt_enable(unsigned int vq_id)
{
    return (vq_id);
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vq_id Vector ID that need to be converted to IRQ number
 *
 * @return vq_id. Return value is never checked.
 *
 */
int platform_interrupt_disable(unsigned int vq_id)
{
    return (vq_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(unsigned int vrt_addr, unsigned int phy_addr, unsigned int size, unsigned int flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate()
{
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable()
{
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
unsigned long platform_vatopa(void *addr)
{
    return ((unsigned long)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(unsigned long addr)
{
    return ((void *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int platform_init(int cpu_id, platform_ops_t *pOps)
{
    if( cpu_id < 4 )
        g_pPlatform_ops[cpu_id] = pOps;
    return 0;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int platform_deinit(int cpu_id)
{
    if( cpu_id < 4 )
        g_pPlatform_ops[cpu_id] = 0;
    return 0;
}


void platform_rpmsg_handler(int vring_idx)
{
    env_isr(vring_idx);
}
