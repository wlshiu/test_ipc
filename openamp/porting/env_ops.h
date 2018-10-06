/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
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
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
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
/** @file env_ops.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/06
 * @license
 * @description
 */

#ifndef __env_ops_H_wxPOPgek_lzyq_Hr21_slQi_uzSZL9XNYyBL__
#define __env_ops_H_wxPOPgek_lzyq_Hr21_slQi_uzSZL9XNYyBL__

#ifdef __cplusplus
extern "C" {
#endif


//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct env_ops
{
    /**
     * -------------------------------------------------------------------------
     *
     * Dynamic memory management functions. The parameters
     * are similar to standard c functions.
     *
     *-------------------------------------------------------------------------
     **/

    /**
     * env_allocate_memory
     *
     * Allocates memory with the given size.
     *
     * @param size - size of memory to allocate
     *
     * @return - pointer to allocated memory
     */
    void* (*pf_malloc)(unsigned int size);

    /**
     * env_free_memory
     *
     * Frees memory pointed by the given parameter.
     *
     * @param ptr - pointer to memory to free
     */
    void (*pf_free)(void *ptr);

    /**
     * -------------------------------------------------------------------------
     *
     * RTL Functions
     *
     *-------------------------------------------------------------------------
     */

    void (*pf_memset)(void *, int, unsigned long);
    void (*pf_memcpy)(void *, void const *, unsigned long);
    size_t (*pf_strlen)(const char *);
    void (*pf_strcpy)(char *, const char *);
    int (*pf_strcmp)(const char *, const char *);
    void (*pf_strncpy)(char *, const char *, unsigned long);
    int (*pf_strncmp)(char *, const char *, unsigned long);

    /**
     *-----------------------------------------------------------------------------
     *
     *  Functions to convert physical address to virtual address and vice versa.
     *
     *-----------------------------------------------------------------------------
     */

    /**
     * env_map_vatopa
     *
     * Converts logical address to physical address
     *
     * @param address - pointer to logical address
     *
     * @return  - physical address
     */
    unsigned long (*pf_map_vatopa)(void *address);

    /**
     * env_map_patova
     *
     * Converts physical address to logical address
     *
     * @param address - pointer to physical address
     *
     * @return  - logical address
     *
     */
    void *(*pf_map_patova)(unsigned long address);

    /**
     *-----------------------------------------------------------------------------
     *
     *  Abstractions for memory barrier instructions.
     *
     *-----------------------------------------------------------------------------
     */

    /**
     * env_mb
     *
     * Inserts memory barrier.
     */

    void (*pf_mb)(void);

    /**
     * env_rmb
     *
     * Inserts read memory barrier
     */

    void (*pf_rmb)(void);

    /**
     * env_wmb
     *
     * Inserts write memory barrier
     */

    void (*pf_wmb)(void);

    /**
     *-----------------------------------------------------------------------------
     *
     *  Abstractions for OS lock primitives.
     *
     *-----------------------------------------------------------------------------
     */

    /**
     * env_create_mutex
     *
     * Creates a mutex with given initial count.
     *
     * @param lock -  pointer to created mutex
     * @param count - initial count 0 or 1
     *
     * @return - status of function execution
     */
    int (*pf_create_mutex)(void **lock , int count);

    /**
     * env_delete_mutex
     *
     * Deletes the given lock.
     *
     * @param lock - mutex to delete
     */

    void (*pf_delete_mutex)(void *lock);

    /**
     * env_lock_mutex
     *
     * Tries to acquire the lock, if lock is not available then call to
     * this function will suspend.
     *
     * @param lock - mutex to lock
     *
     */

    void (*pf_lock_mutex)(void *lock);

    /**
     * env_unlock_mutex
     *
     * Releases the given lock.
     *
     * @param lock - mutex to unlock
     */

    void (*pf_unlock_mutex)(void *lock);

    /**
     * env_create_sync_lock
     *
     * Creates a synchronization lock primitive. It is used
     * when signal has to be sent from the interrupt context to main
     * thread context.
     *
     * @param lock  - pointer to created sync lock object
     * @param state - initial state , lock or unlocked
     *
     * @returns - status of function execution
     */
    int (*pf_create_sync_lock)(void **lock , int state);

    /**
     * env_create_sync_lock
     *
     * Deletes given sync lock object.
     *
     * @param lock  - sync lock to delete.
     *
     */

    void (*pf_delete_sync_lock)(void *lock);


    /**
     * env_acquire_sync_lock
     *
     * Tries to acquire the sync lock.
     *
     * @param lock  - sync lock to acquire.
     */
    void (*pf_acquire_sync_lock)(void *lock);

    /**
     * env_release_sync_lock
     *
     * Releases synchronization lock.
     *
     * @param lock  - sync lock to release.
     */
    void (*pf_release_sync_lock)(void *lock);

    /**
     * env_sleep_msec
     *
     * Suspends the calling thread for given time in msecs.
     *
     * @param num_msec -  delay in msecs
     */
    void (*pf_sleep_msec)(int num_msec);

    /**
     * env_disable_interrupts
     *
     * Disables system interrupts
     *
     */
    void (*pf_disable_interrupts)(void);

    /**
     * env_restore_interrupts
     *
     * Enables system interrupts
     *
     */
    void (*pf_restore_interrupts)(void);

    /**
     * env_register_isr
     *
     * Registers interrupt handler for the given interrupt vector.
     *
     * @param vector - interrupt vector number
     * @param data   - private data
     * @param isr    - interrupt handler
     */

    void (*pf_register_isr)(int vector, void *data,
                            void (*isr)(int vector, void *data));

    void (*pf_update_isr)(int vector, void *data,
                            void (*isr)(int vector, void *data));

    /**
     * env_enable_interrupt
     *
     * Enables the given interrupt.
     *
     * @param vector   - interrupt vector number
     * @param priority - interrupt priority
     * @param polarity - interrupt polarity
     */

    void (*pf_enable_interrupt)(unsigned int vector,
                                unsigned int priority,
                                unsigned int polarity);

    /**
     * env_disable_interrupt
     *
     * Disables the given interrupt.
     *
     * @param vector   - interrupt vector number
     */

    void (*pf_disable_interrupt)(unsigned int vector);

    /**
     * env_map_memory
     *
     * Enables memory mapping for given memory region.
     *
     * @param pa   - physical address of memory
     * @param va   - logical address of memory
     * @param size - memory size
     * param flags - flags for cache/uncached  and access type
     *
     * Currently only first byte of flag parameter is used and bits mapping is defined as follow;
     *
     * Cache bits
     * 0x0000_0001 = No cache
     * 0x0000_0010 = Write back
     * 0x0000_0100 = Write through
     * 0x0000_x000 = Not used
     *
     * Memory types
     *
     * 0x0001_xxxx = Memory Mapped
     * 0x0010_xxxx = IO Mapped
     * 0x0100_xxxx = Shared
     * 0x1000_xxxx = TLB
     */
    void (*pf_map_memory)(unsigned int pa, unsigned int va, unsigned int size,
                            unsigned int flags);

    /**
     * env_get_timestamp
     *
     * Returns a 64 bit time stamp.
     *
     *
     */
    unsigned long long (*pf_get_timestamp)(void);

    /**
     * env_disable_cache
     *
     * Disables system caches.
     *
     */

    void (*pf_disable_cache)(void);

    /**
     * env_create_queue
     *
     * Creates a message queue.
     *
     * @param queue -  pointer to created queue
     * @param length -  maximum number of elements in the queue
     * @param item_size - queue element size in bytes
     *
     * @return - status of function execution
     */
    int (*pf_create_queue)(void **queue, int length , int element_size);

    /**
     * env_delete_queue
     *
     * Deletes the message queue.
     *
     * @param queue - queue to delete
     */

    void (*pf_delete_queue)(void *queue);

    /**
     * env_put_queue
     *
     * Put an element in a queue.
     *
     * @param queue - queue to put element in
     * @param msg - pointer to the message to be put into the queue
     * @param timeout_ms - timeout in ms
     *
     * @return - status of function execution
     */

    int (*pf_put_queue)(void *queue, void* msg, int timeout_ms);

    /**
     * env_get_queue
     *
     * Get an element out of a queue.
     *
     * @param queue - queue to get element from
     * @param msg - pointer to a memory to save the message
     * @param timeout_ms - timeout in ms
     *
     * @return - status of function execution
     */

    int (*pf_get_queue)(void *queue, void* msg, int timeout_ms);

    /**
     * env_isr
     *
     * Invoke RPMSG/IRQ callback
     *
     * @param vector - RPMSG IRQ vector ID.
     */

    void (*pf_isr)(int vector);

    void    *pPrivate_info;

} env_ops_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================

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
