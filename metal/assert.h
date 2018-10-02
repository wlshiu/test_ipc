/*
 * Copyright (c) 2018, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	assert.h
 * @brief	Assertion support.
 */

#ifndef __METAL_ASSERT__H__
#define __METAL_ASSERT__H__

#if !defined(WIN32)
#include <metal/system/@PROJECT_SYSTEM@/assert.h>
#else

#include <stdio.h>
#define metal_sys_assert(a)     do{if(a) break; printf("%s[%d] err: %s\n", __FILE__, __LINE__, #a); while(1);}while(0)

#endif

/**
 * @brief Assertion macro.
 * @param cond Condition to test.
 */
#define metal_assert(cond) metal_sys_assert(cond)

#endif /* __METAL_ASSERT_H__ */

