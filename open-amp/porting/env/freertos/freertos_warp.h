/**
 * Copyright (c) 2018 Wei-Lun Hsu. All Rights Reserved.
 */
/** @file freertos_warp.h
 *
 * @author Wei-Lun Hsu
 * @version 0.1
 * @date 2018/05/17
 * @license
 * @description
 */

#ifndef __freertos_warp_H_wGrhvS3G_ltTb_HWFL_sryW_u0yncbA0Fty6__
#define __freertos_warp_H_wGrhvS3G_ltTb_HWFL_sryW_u0yncbA0Fty6__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>
#include "pthread.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
#define portMAX_DELAY       (-1)
#define portTICK_PERIOD_MS  (1)

#define portCHAR		    char
#define portFLOAT		    float
#define portDOUBLE		    double
#define portLONG		    long
#define portSHORT		    short
#define portSTACK_TYPE	    unsigned long
#define portBASE_TYPE	    long
#define pdFALSE	            false
#define pdTRUE	            true

#define pdPASS			    pdTRUE
#define pdFAIL			    pdFALSE

typedef long                BaseType_t;
typedef unsigned long       UBaseType_t;
typedef unsigned long       TickType_t;

typedef void (*TaskFunction_t)(void *);

typedef uint32_t            SemaphoreHandle_t;
//=============================================================================
//                  Macro Definition
//=============================================================================
#define pvPortMalloc                        malloc
#define vPortFree                           free

#define vTaskDelete(xTaskToDelete)          vTaskDelete_wrap(0)
#define vTaskSuspend(xTaskToSuspend)        vTaskSuspend_wrap(&xTaskToSuspend)
#define vTaskResume(xTaskToResume)          vTaskResume_wrap(&xTaskToResume)

#define xSemaphoreCreateBinary()            xSemaphoreCreateMutex()
#define vTaskSuspendAll()
#define xTaskResumeAll()

#define xSemaphoreCreateCounting(uxMaxCount, uxInitialCount)    xSemaphoreCreateMutex()

#define pdMS_TO_TICKS(a)                    (1)

#define no_impl(a)                           do{ printf("%s[%d]\n", __func__, __LINE__); if(a) while(1);}while(0)

#if 1
//#undef assert
#define assert(expression)                                                  \
        do{ if(expression) break;                                           \
            printf("%s[%d] err: %s\n", __FILE__, __LINE__, #expression);    \
            while(1);                                                       \
        }while(0)
#endif
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct my_task_info
{
    pthread_t           hThread;
    pthread_mutex_t     mtx;
    pthread_cond_t      cond;
} my_task_info_t;

typedef my_task_info_t      TaskHandle_t;


typedef struct my_queue_info
{
    uint32_t        hQueue;
    uint32_t        node_num;
    uint32_t        node_len;
} my_queue_info_t;

typedef my_queue_info_t*    QueueHandle_t;

//=============================================================================
//                  Global Data Definition
//=============================================================================

//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int
init_rtos_wrap(void);


BaseType_t
xTaskCreate(
    TaskFunction_t  pxTaskCode,
    const char      *pcName,
    const uint16_t  usStackDepth,
    void            *pvParameters,
    UBaseType_t     uxPriority,
    TaskHandle_t    *pxCreatedTask);


void
vTaskDelete_wrap(
    TaskHandle_t    *xTaskToDelete);


void
vTaskSuspend_wrap(
    TaskHandle_t    *xTaskToSuspend);


void
task_event_wait(
    TaskHandle_t    *xTaskToSuspend);


void
vTaskResume_wrap(
    TaskHandle_t    *xTaskToResume);


void
vTaskDelay(
    const TickType_t    xTicksToDelay);


TickType_t
xTaskGetTickCount(void);


QueueHandle_t
xQueueCreate(
    const UBaseType_t   uxQueueLength,
    const UBaseType_t   uxItemSize);


void vQueueDelete(
    QueueHandle_t   xQueue);


BaseType_t
xQueueSend(
    QueueHandle_t   xQueue,
    void            *pvItemToQueue,
    TickType_t      xTicksToWait);


BaseType_t
xQueueReceive(
    QueueHandle_t   xQueue,
    void            *pvBuffer,
    TickType_t      xTicksToWait);


UBaseType_t
uxQueueMessagesWaiting(
    QueueHandle_t xQueue);


SemaphoreHandle_t
xSemaphoreCreateMutex(void);


void
vSemaphoreDelete(
    SemaphoreHandle_t   xSemaphore);


BaseType_t
xSemaphoreTake(
    SemaphoreHandle_t   xSemaphore,
    TickType_t          xBlockTime);


BaseType_t
xSemaphoreGive(
    SemaphoreHandle_t   xSemaphore);


#ifdef __cplusplus
}
#endif

#endif
