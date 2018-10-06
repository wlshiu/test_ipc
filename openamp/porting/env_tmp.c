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
 * SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************
 * FILE NAME
 *
 *       env.c
 *
 *
 * DESCRIPTION
 *
 *       This file is the interface of env layer for OpenAMP.
 *
 *
 **************************************************************************/


#include <stdio.h>
#include <string.h>
#include "env.h"

extern env_ops_t       g_env_win32;


#define CONFIG_MAX_PROCESSOR_NUM           2

static env_handle_t         g_evn_handle[CONFIG_MAX_PROCESSOR_NUM] = {0};

/**
 * env_init
 *
 * Initializes OS/BM environment.
 *
 * @returns - execution status
 */
int env_init(int cpu_id)
{
    int     rval = 0;
    do {
        if( cpu_id >= CONFIG_MAX_PROCESSOR_NUM )
            break;

        if( cpu_id == 0 )
            g_evn_handle[cpu_id].ops = g_env_win32;
        else if( cpu_id == 1 )
            g_evn_handle[cpu_id].ops = g_env_win32;


    } while(0);
    return rval;
}

/**
 * env_deinit
 *
 * Uninitializes OS/BM environment.
 *
 * @returns - execution status
 */

int env_deinit(int cpu_id)
{
    int     rval = 0;
    do {
        if( cpu_id >= CONFIG_MAX_PROCESSOR_NUM )
            break;

        memset(&g_evn_handle[cpu_id], 0x0, sizeof(env_handle_t));
    } while(0);
    return rval;
}

env_handle_t*
env_get_handle(int cpu_id)
{
    env_handle_t        *pHandle = 0;
    do {
        if( cpu_id >= CONFIG_MAX_PROCESSOR_NUM )
            break;

        pHandle = &g_evn_handle[cpu_id];
    } while(0);
    return pHandle;
}

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
void *env_allocate_memory(env_handle_t *pHendle, unsigned int size)
{
    void     *ptr = 0;
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_malloc )
            ptr = pHendle->ops.pf_malloc(size);

    } while(0);
    return ptr;
}

/**
 * env_free_memory
 *
 * Frees memory pointed by the given parameter.
 *
 * @param ptr - pointer to memory to free
 */
void env_free_memory(env_handle_t *pHendle, void *ptr)
{
    do {
        if( !pHendle || !ptr )      break;

        if( pHendle->ops.pf_free )
            pHendle->ops.pf_free(ptr);

    } while(0);
    return;
}

/**
 * -------------------------------------------------------------------------
 *
 * RTL Functions
 *
 *-------------------------------------------------------------------------
 */

void env_memset(env_handle_t *pHendle, void *ptr, int value, unsigned long size)
{
    do {
        if( !pHendle || !ptr || !size )      break;

        if( pHendle->ops.pf_memset )
            pHendle->ops.pf_memset(ptr, value, size);
        else
            memset(ptr, value, size);

    } while(0);
    return;
}

void env_memcpy(env_handle_t *pHendle, void *dst, void const *src, unsigned long len)
{
    do {
        if( !pHendle || !dst || !src || len )      break;

        if( pHendle->ops.pf_memcpy )
            pHendle->ops.pf_memcpy(dst, src, len);
        else
            memcpy(dst, src, len);

    } while(0);
    return;
}

size_t env_strlen(env_handle_t *pHendle, const char *str)
{
    size_t      len = 0;
    do {
        if( !pHendle || !str )      break;

        if( pHendle->ops.pf_strlen )
            len = pHendle->ops.pf_strlen(str);
        else
            len = strlen(str);

    } while(0);
    return len;
}

void env_strcpy(env_handle_t *pHendle, char * dest, const char *src)
{
    do {
        if( !pHendle || !dest || !src )      break;

        if( pHendle->ops.pf_strcpy )
            pHendle->ops.pf_strcpy(dest, src);
        else
            strcpy(dest, src);

    } while(0);
    return;
}

int env_strcmp(env_handle_t *pHendle, const char *dst, const char *src)
{
    int     rval = 1;
    do {
        if( !pHendle || !dst || !src )      break;

        if( pHendle->ops.pf_strcmp )
            rval = pHendle->ops.pf_strcmp(dst, src);
        else
            rval = strcmp(dst, src);

    } while(0);
    return rval;
}

void env_strncpy(env_handle_t *pHendle, char *dest, const char *src, unsigned long len)
{
    do {
        if( !pHendle || !dest || !src || !len )      break;

        if( pHendle->ops.pf_strncpy )
            pHendle->ops.pf_strncpy(dest, src, len);
        else
            strncpy(dest, src, len);

    } while(0);
    return;
}

int env_strncmp(env_handle_t *pHendle, char *dest, const char *src, unsigned long len)
{
    int     rval = 1;
    do {
        if( !pHendle || !dest || !src || len )      break;

        if( pHendle->ops.pf_strncmp )
            rval = pHendle->ops.pf_strncmp(dest, src, len);
        else
            rval = strncmp(dest, src, len);

    } while(0);
    return rval;
}

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
unsigned long env_map_vatopa(env_handle_t *pHendle, void *address)
{
    unsigned long     ptr = (unsigned long)address;
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_map_vatopa )
            ptr = pHendle->ops.pf_map_vatopa(address);

    } while(0);
    return ptr;
}

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
void *env_map_patova(env_handle_t *pHendle, unsigned long address)
{
    void     *ptr = (void*)address;
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_map_patova )
            ptr = pHendle->ops.pf_map_patova(address);

    } while(0);
    return ptr;
}

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

void env_mb(env_handle_t *pHendle)
{
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_mb )
            pHendle->ops.pf_mb();

    } while(0);
    return;
}

/**
 * env_rmb
 *
 * Inserts read memory barrier
 */

void env_rmb(env_handle_t *pHendle)
{
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_rmb )
            pHendle->ops.pf_rmb();

    } while(0);
    return;
}

/**
 * env_wmb
 *
 * Inserts write memory barrier
 */

void env_wmb(env_handle_t *pHendle)
{
    do {
        if( !pHendle )      break;

        if( pHendle->ops.pf_wmb )
            pHendle->ops.pf_wmb();

    } while(0);
    return;
}

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
int env_create_mutex(env_handle_t *pHendle, void **lock , int count)
{
    int     rval = 0;
    do {
        if( !pHendle || lock )
        {
            rval = -1;
            break;
        }

        if( pHendle->ops.pf_create_mutex )
            rval = pHendle->ops.pf_create_mutex(lock, count);

    } while(0);
    return rval;
}

/**
 * env_delete_mutex
 *
 * Deletes the given lock.
 *
 * @param lock - mutex to delete
 */

void env_delete_mutex(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_delete_mutex )
            pHendle->ops.pf_delete_mutex(lock);

    } while(0);
    return;
}

/**
 * env_lock_mutex
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 *
 * @param lock - mutex to lock
 *
 */

void env_lock_mutex(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_lock_mutex )
            pHendle->ops.pf_lock_mutex(lock);

    } while(0);
    return;
}

/**
 * env_unlock_mutex
 *
 * Releases the given lock.
 *
 * @param lock - mutex to unlock
 */

void env_unlock_mutex(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_unlock_mutex )
            pHendle->ops.pf_unlock_mutex(lock);

    } while(0);
    return;
}

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
int env_create_sync_lock(env_handle_t *pHendle, void **lock , int state)
{
    int     rval = 0;
    do {
        if( !pHendle || lock )
        {
            rval = -1;
            break;
        }

        if( pHendle->ops.pf_create_sync_lock )
            rval = pHendle->ops.pf_create_sync_lock(lock, state);

    } while(0);
    return rval;
}

/**
 * env_create_sync_lock
 *
 * Deletes given sync lock object.
 *
 * @param lock  - sync lock to delete.
 *
 */

void env_delete_sync_lock(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_delete_sync_lock )
            pHendle->ops.pf_delete_sync_lock(lock);

    } while(0);
    return;
}


/**
 * env_acquire_sync_lock
 *
 * Tries to acquire the sync lock.
 *
 * @param lock  - sync lock to acquire.
 */
void env_acquire_sync_lock(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_acquire_sync_lock )
            pHendle->ops.pf_acquire_sync_lock(lock);

    } while(0);
    return;
}

/**
 * env_release_sync_lock
 *
 * Releases synchronization lock.
 *
 * @param lock  - sync lock to release.
 */
void env_release_sync_lock(env_handle_t *pHendle, void *lock)
{
    do {
        if( !pHendle || lock )  break;

        if( pHendle->ops.pf_release_sync_lock )
            pHendle->ops.pf_release_sync_lock(lock);

    } while(0);
    return;
}

/**
 * env_sleep_msec
 *
 * Suspends the calling thread for given time in msecs.
 *
 * @param num_msec -  delay in msecs
 */
void env_sleep_msec(env_handle_t *pHendle, int num_msec)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_sleep_msec )
            pHendle->ops.pf_sleep_msec(num_msec);

    } while(0);
    return;
}

/**
 * env_disable_interrupts
 *
 * Disables system interrupts
 *
 */
void env_disable_interrupts(env_handle_t *pHendle)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_disable_interrupts )
            pHendle->ops.pf_disable_interrupts();

    } while(0);
    return;
}

/**
 * env_restore_interrupts
 *
 * Enables system interrupts
 *
 */
void env_restore_interrupts(env_handle_t *pHendle)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_restore_interrupts )
            pHendle->ops.pf_restore_interrupts();

    } while(0);
    return;
}

/**
 * env_register_isr
 *
 * Registers interrupt handler for the given interrupt vector.
 *
 * @param vector - interrupt vector number
 * @param data   - private data
 * @param isr    - interrupt handler
 */

void env_register_isr(env_handle_t *pHendle, int vector, void *data,
                    void (*isr)(int vector, void *data))
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_register_isr )
            pHendle->ops.pf_register_isr(vector, data, isr);

    } while(0);
    return;
}

void env_update_isr(env_handle_t *pHendle, int vector, void *data,
                    void (*isr)(int vector, void *data))
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_update_isr )
            pHendle->ops.pf_update_isr(vector, data, isr);

    } while(0);
    return;
}

/**
 * env_enable_interrupt
 *
 * Enables the given interrupt.
 *
 * @param vector   - interrupt vector number
 * @param priority - interrupt priority
 * @param polarity - interrupt polarity
 */

void env_enable_interrupt(env_handle_t *pHendle, unsigned int vector, unsigned int priority,
                            unsigned int polarity)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_enable_interrupt )
            pHendle->ops.pf_enable_interrupt(vector, priority, polarity);

    } while(0);
    return;
}

/**
 * env_disable_interrupt
 *
 * Disables the given interrupt.
 *
 * @param vector   - interrupt vector number
 */

void env_disable_interrupt(env_handle_t *pHendle, unsigned int vector)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_disable_interrupt )
            pHendle->ops.pf_disable_interrupt(vector);

    } while(0);
    return;
}

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
 * Currently only first byte of flag parameter is used and bits mapping is defined as follow
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
void env_map_memory(env_handle_t *pHendle, unsigned int pa, unsigned int va, unsigned int size,
                unsigned int flags)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_map_memory )
            pHendle->ops.pf_map_memory(pa, va, size, flags);

    } while(0);
    return;
}

/**
 * env_get_timestamp
 *
 * Returns a 64 bit time stamp.
 *
 *
 */
unsigned long long env_get_timestamp(env_handle_t *pHendle)
{
    unsigned long long      ts = 0ull;
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_get_timestamp )
            ts = pHendle->ops.pf_get_timestamp();

    } while(0);
    return ts;
}

/**
 * env_disable_cache
 *
 * Disables system caches.
 *
 */

void env_disable_cache(env_handle_t *pHendle)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_disable_cache )
            pHendle->ops.pf_disable_cache();

    } while(0);
    return;
}


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
int env_create_queue(env_handle_t *pHendle, void **queue, int length, int element_size)
{
    int     rval = 0;
    do {
        if( !pHendle || !queue )
        {
            rval = -1;
            break;
        }

        if( pHendle->ops.pf_create_queue )
            rval = pHendle->ops.pf_create_queue(queue, length, element_size);

    } while(0);
    return rval;
}

/**
 * env_delete_queue
 *
 * Deletes the message queue.
 *
 * @param queue - queue to delete
 */

void env_delete_queue(env_handle_t *pHendle, void *queue)
{
    do {
        if( !pHendle || !queue )    break;

        if( pHendle->ops.pf_delete_queue )
            pHendle->ops.pf_delete_queue(queue);

    } while(0);
    return;
}

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

int env_put_queue(env_handle_t *pHendle, void *queue, void* msg, int timeout_ms)
{
    int     rval = 0;
    do {
        if( !pHendle || !queue || !msg )
        {
            rval = -1;
            break;
        }

        if( pHendle->ops.pf_put_queue )
            rval = pHendle->ops.pf_put_queue(queue, msg, timeout_ms);

    } while(0);
    return rval;
}

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

int env_get_queue(env_handle_t *pHendle, void *queue, void* msg, int timeout_ms)
{
    int     rval = 0;
    do {
        if( !pHendle || !queue || !msg )
        {
            rval = -1;
            break;
        }

        if( pHendle->ops.pf_get_queue )
            rval = pHendle->ops.pf_get_queue(queue, msg, timeout_ms);

    } while(0);
    return rval;
}

/**
 * env_isr
 *
 * Invoke RPMSG/IRQ callback
 *
 * @param vector - RPMSG IRQ vector ID.
 */

void env_isr(env_handle_t *pHendle, int vector)
{
    do {
        if( !pHendle )  break;

        if( pHendle->ops.pf_isr )
            pHendle->ops.pf_isr(vector);

    } while(0);
    return;
}

