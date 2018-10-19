/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file env_win32.c
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/10/06
 * @license
 * @description
 */



#include "env.h"
#include <stdlib.h>
#include <string.h>
#include "pthread.h"

#include <windows.h>

//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================
/**
 * env_init
 *
 * Initializes OS/BM environment.
 *
 * @returns - execution status
 */
int env_init(void)
{
    int     rval = 0;
    return rval;
}

/**
 * env_deinit
 *
 * Uninitializes OS/BM environment.
 *
 * @returns - execution status
 */

int env_deinit(void)
{
    int     rval = 0;
    return rval;
}

/**
 * env_allocate_memory - implementation
 *
 * @param size
 */
void *env_allocate_memory(unsigned int size)
{
    return (malloc(size));
}

/**
 * env_free_memory - implementation
 *
 * @param ptr
 */
void env_free_memory(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
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
 * env_map_vatopa - implementation
 *
 * @param address
 */
unsigned long env_map_vatopa(void *address)
{
    return (unsigned long)address;
}

/**
 * env_map_patova - implementation
 *
 * @param address
 */
void *env_map_patova(unsigned long address)
{
    return (void*)address;
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

void env_mb(void)
{
    return;
}

/**
 * env_rmb
 *
 * Inserts read memory barrier
 */

void env_rmb(void)
{
    return;
}

/**
 * env_wmb
 *
 * Inserts write memory barrier
 */

void env_wmb(void)
{
    return;
}


/**
 * env_create_mutex
 *
 * Creates a mutex with the given initial count.
 *
 */
int env_create_mutex(void **lock, int count)
{
    pthread_mutexattr_t     mtx_attr;
    pthread_mutex_t         *pMutex = malloc(sizeof(pthread_mutex_t));
    memset(pMutex, 0x0, sizeof(pthread_mutex_t));

    pthread_mutexattr_init(&mtx_attr);
//    pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(pMutex, &mtx_attr);
    *lock = pMutex;
    return 0;
}

/**
 * env_delete_mutex
 *
 * Deletes the given lock
 *
 */
void env_delete_mutex(void *lock)
{
    if( lock )
    {
        pthread_mutex_destroy((pthread_mutex_t*)lock);
        free(lock);
    }
}

/**
 * env_lock_mutex
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 */
void env_lock_mutex(void *lock)
{
    pthread_mutex_lock(lock);
}

/**
 * env_unlock_mutex
 *
 * Releases the given lock.
 */
void env_unlock_mutex(void *lock)
{
    pthread_mutex_unlock(lock);
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
    /* TODO */
    return 0;
}

/**
 * env_delete_sync_lock
 *
 * Deletes the given lock
 *
 */
void env_delete_sync_lock(void *lock)
{
    /* TODO */
}

/**
 * env_acquire_sync_lock
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function waits for lock to become available.
 */
void env_acquire_sync_lock(void *lock)
{
    /* TODO */
}

/**
 * env_release_sync_lock
 *
 * Releases the given lock.
 */
void env_release_sync_lock(void *lock)
{
    /* TODO */
}

/**
 * env_sleep_msec
 *
 * Suspends the calling thread for given time , in msecs.
 */
void env_sleep_msec(int num_msec)
{
    Sleep(num_msec);
}

/**
 * env_disable_interrupts
 *
 * Disables system interrupts
 *
 */
void env_disable_interrupts()
{
}

/**
 * env_restore_interrupts
 *
 * Enables system interrupts
 *
 */
void env_restore_interrupts()
{
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

}

void env_update_isr(int vector, void *data,
                    void (*isr)(int vector, void *data))
{

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
}

/**
 * env_disable_cache
 *
 * Disables system caches.
 *
 */

void env_disable_cache()
{
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

    /* TODO: Provide implementation for baremetal*/
    return 0;
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
int env_create_queue(void **queue, int length , int element_size)
{
    return 0;
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

int env_put_queue(void *queue, void* msg, int timeout_ms)
{
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

int env_get_queue(void *queue, void* msg, int timeout_ms)
{
    return 0;
}

/*!
 * env_get_current_queue_size
 *
 * Get current queue size.
 *
 * @param queue - queue pointer
 *
 * @return - Number of queued items in the queue
 */

int env_get_current_queue_size(void *queue)
{
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================

