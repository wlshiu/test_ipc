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
 *       platform_info.h
 *
 * COMPONENT
 *
 *       OpenAMP
 *
 * DESCRIPTION
 *
 * This file provides function prototypes of platform.c.
 *
 **************************************************************************/

#ifndef platform_info_H_
#define platform_info_H_

#include "common/hil/hil.h"

/* IPI_VECT here defines VRING index */
typedef enum vring_vect
{
    VRING_VECT_0        = 0,
    VRING_VECT_1,

    VRING_VECT_TOTAL
} vring_vect_t;


int platform_get_processor_info(struct hil_proc *proc, int cpu_id);

int platform_get_processor_for_fw(char *fw_name);

int platform_set_ops(
    unsigned long               cpu_id,
    struct hil_platform_ops     *pOps);


int
platform_set_vring_intr_priv_data(
    unsigned long   cpu_id,
    void            *pPriv_data);


void
platform_vring_isr(
    int     vect_id,
    void    *data);


#endif /* PLATFORM_H_ */
