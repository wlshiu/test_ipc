/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	cpu.h
 * @brief	CPU primitives for libmetal.
 */

#ifndef __METAL_CPU__H__
#define __METAL_CPU__H__

#if !defined(WIN32)
# include <metal/processor/@PROJECT_PROCESSOR@/cpu.h>
#else
    
#define metal_cpu_yield()

#endif
#endif /* __METAL_CPU__H__ */
