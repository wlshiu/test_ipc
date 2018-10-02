
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <windows.h>


#include "core_info.h"
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
core_attr_t         g_core_attr[CORE_ID_TOTAL];
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void
_isr_core_0(void)
{
    return;
}

static void
_isr_core_1(void)
{
    return;
}

static void*
_task_core_0(void *argv)
{
    core_attr_t     *pCore_attr = (core_attr_t*)argv;

    while(1)
    {

        Sleep((rand() >> 5)& 0x3);
    }

    pthread_exit(0);
    return 0;
}

static void*
_task_core_1(void *argv)
{

    while(1)
    {


        Sleep((rand() >> 5)& 0x3);
    }
    pthread_exit(0);
    return 0;
}
//=============================================================================
//                  Public Function Definition
//=============================================================================
int main()
{
    pthread_t   t0, t1;

    srand(clock());


    memset(&g_core_attr, 0x0, sizeof(g_core_attr));

    g_core_attr[CORE_ID_0].core_id = CORE_ID_0;
    g_core_attr[CORE_ID_0].pf_irs  = _isr_core_0;

    g_core_attr[CORE_ID_1].core_id = CORE_ID_1;
    g_core_attr[CORE_ID_1].pf_irs  = _isr_core_1;

    core_irq_simulator(&g_core_attr[0], CORE_ID_TOTAL);


    //------------------------------
    // start
    pthread_create(&t0, 0, _task_core_0, &g_core_attr[CORE_ID_0]);
    pthread_create(&t1, 0, _task_core_1, &g_core_attr[CORE_ID_1]);


    pthread_join(t0, 0);
    pthread_join(t1, 0);

    return 0;
}
