/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Philip Thrasher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 *
 * Philip Thrasher's Crazy Awesome Ring Buffer Macros!
 *
 * Below you will find some naughty macros for easy owning and manipulating
 * generic ring buffers. Yes, they are slightly evil in readability, but they
 * are really fast, and they work great.
 *
 */

#if 0
#include <stdio.h>
#include <stdlib.h>
#include "rbuf.h"

RBUF_TYPEDEF(char, node_t);

int main()
{
    node_t      handle;
    char        buf_pool[3] = {0};
    char        push_node = 0;
    char        pop_node = 0;

    RBUF_INIT(&handle, 3, buf_pool, char);

    for(int i = 0; i < 4; i++)
    {
        push_node = i;
        RBUF_PUSH(&handle, push_node);
    }

    for(int i = 0; i < 3; i++)
    {
        RBUF_POP(&handle, pop_node);
        printf("%d\n", pop_node);
    }

    return 0;
}
#endif // 0

#ifndef ___ringbuffer_h
#define ___ringbuffer_h

#ifdef __cplusplus
extern "C" {
#endif

#define RBUF_NEXT_START_IDX(pHBuf)       (((pHBuf)->start + 1) % ((pHBuf)->size + 1))
#define RBUF_NEXT_END_IDX(pHBuf)         (((pHBuf)->end + 1) % ((pHBuf)->size + 1))
#define RBUF_IS_EMPTY(pHBuf)             ((pHBuf)->end == (pHBuf)->start)
#define RBUF_IS_FULL(pHBuf)              (RBUF_NEXT_END_IDX(pHBuf) == (pHBuf)->start)


#define RBUF_TYPEDEF(T, NAME) \
    typedef struct { \
        int size, start, end; \
        T* elems; \
    } NAME

#define RBUF_INIT(pHBuf, count, pBuf_pool, T) \
    do{ (pHBuf)->size = count; \
        (pHBuf)->start = (pHBuf)->end = 0; \
        (pHBuf)->elems = (T*)pBuf_pool;\
    }while(0)


#define RBUF_PUSH(pHBuf, ELEM) \
    do{ (pHBuf)->elems[(pHBuf)->end] = ELEM; \
        (pHBuf)->end = RBUF_NEXT_END_IDX(pHBuf); \
        if (RBUF_IS_EMPTY(pHBuf)) { \
            (pHBuf)->start = RBUF_NEXT_START_IDX(pHBuf); \
        } \
    }while(0)

#define RBUF_POP(pHBuf, ELEM) \
    do{ if (RBUF_IS_EMPTY(pHBuf))   break;\
        ELEM = (pHBuf)->elems[(pHBuf)->start]; \
        (pHBuf)->start = RBUF_NEXT_START_IDX(pHBuf); \
    }while(0)


#ifdef __cplusplus
}
#endif

#endif
