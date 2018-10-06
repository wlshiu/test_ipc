/*
 * Copyright (c) 2015 Freescale Semiconductor, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Freescale Semiconductor nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
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

/**************************************************************************
 * FILE NAME
 *
 *       platform_ops.c
 *
 * DESCRIPTION
 *
 *       This file is the Implementation of IPC hardware layer interface
 *
 **************************************************************************/

#include "common/hil/hil.h"

static int _enable_interrupt(struct proc_vring *vring_hw)
{
    return 0;
}

static void _notify(int cpu_id, struct proc_intr *intr_info)
{
    /* As Linux suggests, use MU->Data Channle 1 as communication channel */
    return;
}

static int _boot_cpu(int cpu_id, unsigned int load_addr)
{
    /* not imlemented */
    return 0;
}

static void _shutdown_cpu(int cpu_id)
{
    /* not imlemented */
    return;
}

struct hil_platform_ops     processor_ops = 
{
    .enable_interrupt   = _enable_interrupt,
    .notify             = _notify,
    .boot_cpu           = _boot_cpu,
    .shutdown_cpu       = _shutdown_cpu,
};

/**
 * rpmsg_handler
 *
 * Called directly from ISR. Convert IRQ number to channel
 * number and invoke 'env_isr'.
 *
 */
void rpmsg_isr(void)
{
    return;
}

