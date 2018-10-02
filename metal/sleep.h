/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	sleep.h
 * @brief	Sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#define __METAL_SLEEP__H__

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(WIN32)
#include <metal/system/@PROJECT_SYSTEM@/sleep.h>
#else
#include <windows.h>
static inline int __metal_sleep_usec(unsigned int usec)
{
	Sleep(usec/1000);
	return 0;
}
#endif

/** \defgroup sleep Sleep Interfaces
 *  @{ */

/**
 * @brief      delay in microseconds
 *             delay the next execution in the calling thread
 *             fo usec microseconds.
 *
 * @param[in]  usec      microsecond intervals
 * @return     0 on success, non-zero for failures
 */
static inline int metal_sleep_usec(unsigned int usec)
{
	return __metal_sleep_usec(usec);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_SLEEP__H__ */

