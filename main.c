
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>

extern void init_core_0(void);
extern void init_core_1(void);

pthread_mutex_t     g_mtx;
uint8_t             g_share_buf[2 << 10] = {0};


int main()
{
    pthread_mutex_init(&g_mtx, 0);
    init_core_0();
    init_core_1();

    while(1)    Sleep(5000);
    return 0;
}
