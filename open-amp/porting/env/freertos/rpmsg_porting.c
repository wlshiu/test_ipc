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

/**************************************************************************
 * FILE NAME
 *
 *       freertos_env.c
 *
 *
 * DESCRIPTION
 *
 *       This file is FreeRTOS Implementation of env layer for OpenAMP.
 *
 *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "porting/env/env.h"
#include "porting/config/config.h"

#if 1
    #include "freertos_warp.h"
#else
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
#endif

#include "platform.h"

#if 0//(defined(__CC_ARM))
    #define MEM_BARRIER()      __schedule_barrier()
#elif 0//(defined(__GNUC__))
    #define MEM_BARRIER()      asm volatile("" ::: "memory")
#else
    #define MEM_BARRIER()
#endif




static int              env_init_counter[CORE_ID_TOTAL] = {0};
static struct isr_info  isr_table[CORE_ID_TOTAL][ISR_COUNT];

static uint32_t         g_env_core_uid[CORE_ID_TOTAL] = {0};


static int
_get_core_id()
{
    for(int i = 0; i < CORE_ID_TOTAL; ++i)
    {
        if( g_env_core_uid[i] == pthread_self() )
            return i;
    }
    return -1;
}

int
env_pair_core(
    core_id_t   core_id,
    uint32_t    uid)
{
    g_env_core_uid[core_id] = uid;
    return 0;
}

/**
 * env_in_isr
 *
 * @returns - true, if currently in ISR
 *
 */
int env_in_isr(void)
{
    return platform_in_isr();
}

/**
 * env_init
 *
 * Initializes environment.
 *
 */
int env_init()
{
    int     id = _get_core_id();

    // verify 'env_init_counter'
    assert(env_init_counter[id] >= 0);
    if (env_init_counter[id] < 0)
        return -1;
    env_init_counter[id]++;
    // multiple call of 'env_init' - return ok
    if (1 < env_init_counter[id])
        return 0;
    // first call
    memset(&isr_table[id], 0, sizeof(isr_table[0]));
    return platform_init();
}

/**
 * env_deinit
 *
 * Uninitializes environment.
 *
 * @returns - execution status
 */
int env_deinit()
{
    int     id = _get_core_id();
    // verify 'env_init_counter'
    assert(env_init_counter[id] > 0);
    if (env_init_counter[id] <= 0)
        return -1;
    // counter on zero - call platform deinit
    env_init_counter[id]--;
    // multiple call of 'env_deinit' - return ok
    if (0 < env_init_counter[id])
        return 0;
    // last call
    memset(&isr_table[id], 0, sizeof(isr_table[0]));
    return platform_deinit();
}
/**
 * env_allocate_memory - implementation
 *
 * @param size
 */
void *env_allocate_memory(unsigned int size)
{
    return (pvPortMalloc(size));
}

/**
 * env_free_memory - implementation
 *
 * @param ptr
 */
void env_free_memory(void *ptr)
{
    vPortFree(ptr);
}

/**
 *
 * env_memset - implementation
 *
 * @param ptr
 * @param value
 * @param size
 */
void env_memset(void *ptr, int value, unsigned long size)
{
    memset(ptr, value, size);
}

/**
 *
 * env_memcpy - implementation
 *
 * @param dst
 * @param src
 * @param len
 */
void env_memcpy(void *dst, void const *src, unsigned long len)
{
    memcpy(dst, src, len);
}

/**
 *
 * env_strcmp - implementation
 *
 * @param dst
 * @param src
 */

int env_strcmp(const char *dst, const char *src)
{
    return (strcmp(dst, src));
}

/**
 *
 * env_strncpy - implementation
 *
 * @param dest
 * @param src
 * @param len
 */
void env_strncpy(char *dest, const char *src, unsigned long len)
{
    strncpy(dest, src, len);
}

/**
 *
 * env_strncmp - implementation
 *
 * @param dest
 * @param src
 * @param len
 */
int env_strncmp(char *dest, const char *src, unsigned long len)
{
    return (strncmp(dest, src, len));
}

/**
 *
 * env_mb - implementation
 *
 */
void env_mb()
{
    MEM_BARRIER();
}

/**
 * osalr_mb - implementation
 */
void env_rmb()
{
    MEM_BARRIER();
}

/**
 * env_wmb - implementation
 */
void env_wmb()
{
    MEM_BARRIER();
}

/**
 * env_map_vatopa - implementation
 *
 * @param address
 */
unsigned long env_map_vatopa(void *address)
{
    return platform_vatopa(address);
}

/**
 * env_map_patova - implementation
 *
 * @param address
 */
void *env_map_patova(unsigned long address)
{
    return platform_patova(address);
}

/**
 * env_create_mutex
 *
 * Creates a mutex with the given initial count.
 *
 */
int env_create_mutex(void **lock, int count)
{
    *lock = xSemaphoreCreateCounting(10, count);
    if(*lock)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * env_delete_mutex
 *
 * Deletes the given lock
 *
 */
void env_delete_mutex(void *lock)
{
    vSemaphoreDelete((SemaphoreHandle_t)lock);
}

/**
 * env_lock_mutex
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 */
void env_lock_mutex(void *lock)
{
    SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t)lock;
//    if(!env_in_isr())
    {
        xSemaphoreTake( xSemaphore, portMAX_DELAY);
        platform_interrupt_disable_all();
    }
}

/**
 * env_unlock_mutex
 *
 * Releases the given lock.
 */
void env_unlock_mutex(void *lock)
{
    SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t)lock;
//    if(!env_in_isr())
    {
        platform_interrupt_enable_all();
        xSemaphoreGive( xSemaphore);
    }
}


/**
 * env_create_sync_lock
 *
 * Creates a synchronization lock primitive. It is used
 * when signal has to be sent from the interrupt context to main
 * thread context.
 */
int env_create_sync_lock(void **lock, int state)
{
    return env_create_mutex(lock, state); /* state=1 .. initially free */
}

/**
 * env_delete_sync_lock
 *
 * Deletes the given lock
 *
 */
void env_delete_sync_lock(void *lock)
{
    if(lock)
        env_delete_mutex(lock);
}

/**
 * env_acquire_sync_lock
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function waits for lock to become available.
 */
void env_acquire_sync_lock(void *lock)
{
    SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t)lock;

#if 0
    BaseType_t xTaskWokenByReceive = pdFALSE;
    if(env_in_isr())
    {
        xSemaphoreTakeFromISR(xSemaphore, &xTaskWokenByReceive);
        portEND_SWITCHING_ISR( xTaskWokenByReceive );
    }
    else
#endif
    {
        xSemaphoreTake( xSemaphore, portMAX_DELAY);
    }
}

/**
 * env_release_sync_lock
 *
 * Releases the given lock.
 */
void env_release_sync_lock(void *lock)
{
    SemaphoreHandle_t xSemaphore = (SemaphoreHandle_t)lock;
#if 0
    BaseType_t xTaskWokenByReceive = pdFALSE;
    if(env_in_isr())
    {
        xSemaphoreGiveFromISR(xSemaphore, &xTaskWokenByReceive);
        portEND_SWITCHING_ISR( xTaskWokenByReceive );
    }
    else
#endif
    {
        xSemaphoreGive( xSemaphore);
    }
}

/**
 * env_sleep_msec
 *
 * Suspends the calling thread for given time , in msecs.
 */
void env_sleep_msec(int num_msec)
{
    vTaskDelay(num_msec / portTICK_PERIOD_MS);
}

/**
 * env_disable_interrupts
 *
 * Disables system interrupts
 *
 */
void env_disable_interrupts()
{
    platform_interrupt_disable_all();
}

/**
 * env_restore_interrupts
 *
 * Enables system interrupts
 *
 */
void env_restore_interrupts()
{
    platform_interrupt_enable_all();
}

/**
 * env_register_isr
 *
 * Registers interrupt handler for the given interrupt vector.
 *
 * @param vector - interrupt vector number
 * @param isr    - interrupt handler
 */
void env_register_isr(int vector, void *data,
                      void (*isr)(int vector, void *data))
{
    assert(vector < ISR_COUNT);
    if(vector < ISR_COUNT)
    {
        int     id = _get_core_id();
        /* Save interrupt data */
        isr_table[id][vector].data = data;
        isr_table[id][vector].isr = isr;
    }
}

void env_update_isr(int vector, void *data,
                    void (*isr)(int vector, void *data))
{
    assert(vector < ISR_COUNT);
    if(vector < ISR_COUNT)
    {
        int     id = _get_core_id();
        isr_table[id][vector].data = data;
        isr_table[id][vector].isr = isr;
    }
}

/**
 * env_enable_interrupt
 *
 * Enables the given interrupt
 *
 * @param vector   - interrupt vector number
 * @param priority - interrupt priority
 * @param polarity - interrupt polarity
 */

void env_enable_interrupt(unsigned int vector, unsigned int priority,
                          unsigned int polarity)
{
    assert(vector < ISR_COUNT);
    if (vector < ISR_COUNT)
    {
        int     id = _get_core_id();
        isr_table[id][vector].priority = priority;
        isr_table[id][vector].type = polarity;
        platform_interrupt_enable(vector, polarity, priority);
    }
}

/**
 * env_disable_interrupt
 *
 * Disables the given interrupt
 *
 * @param vector   - interrupt vector number
 */

void env_disable_interrupt(unsigned int vector)
{
    platform_interrupt_disable(vector);
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
 */

void env_map_memory(unsigned int pa, unsigned int va, unsigned int size,
                    unsigned int flags)
{
    platform_map_mem_region(va, pa, size, flags);
}

/**
 * env_disable_cache
 *
 * Disables system caches.
 *
 */

void env_disable_cache()
{
    platform_cache_all_flush_invalidate();
    platform_cache_disable();
}

/**
 *
 * env_get_timestamp
 *
 * Returns a 64 bit time stamp.
 *
 *
 */
unsigned long long env_get_timestamp(void)
{
#if 0
    if(env_in_isr())
    {
        return (unsigned long long) xTaskGetTickCountFromISR();
    }
    else
#endif
    {
        return (unsigned long long) xTaskGetTickCount();
    }
}

/*========================================================= */
/* Util data / functions  */

void env_isr(int vector)
{
    struct isr_info *info;

    assert(vector < ISR_COUNT);
    if (vector < ISR_COUNT)
    {
        int     id = _get_core_id();
        info = &isr_table[id][vector];
        assert(NULL != info->isr);
        if (NULL != info->isr)
        {
            env_disable_interrupt(vector);
            info->isr(vector, info->data);
            env_enable_interrupt(vector, info->priority, info->type);
        }
    }
}

/*
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
int env_create_queue(void **queue, int length, int element_size)
{
    *queue = xQueueCreate(length, element_size);
    if(*queue)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * env_delete_queue
 *
 * Deletes the message queue.
 *
 * @param queue - queue to delete
 */

void env_delete_queue(void *queue)
{
    vQueueDelete(queue);
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

int env_put_queue(void *queue, void *msg, int timeout_ms)
{
#if 0
    BaseType_t xHigherPriorityTaskWoken;
    if(env_in_isr())
    {
        if(xQueueSendFromISR(queue, msg, &xHigherPriorityTaskWoken) == pdPASS)
        {
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
            return 1;
        }
    }
    else
#endif
    {
        if(xQueueSend(queue, msg, ((portMAX_DELAY == timeout_ms) ? portMAX_DELAY : timeout_ms / portTICK_PERIOD_MS)) == pdPASS)
        {
            return 1;
        }
    }
    return 0;
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

int env_get_queue(void *queue, void *msg, int timeout_ms)
{
#if 0
    BaseType_t xHigherPriorityTaskWoken;
    if(env_in_isr())
    {
        if(xQueueReceiveFromISR(queue, msg, &xHigherPriorityTaskWoken) == pdPASS)
        {
            portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
            return 1;
        }
    }
    else
#endif
    {
        if(xQueueReceive(queue, msg, ((portMAX_DELAY == timeout_ms) ? portMAX_DELAY : timeout_ms / portTICK_PERIOD_MS)) == pdPASS)
        {
            return 1;
        }
    }
    return 0;
}
