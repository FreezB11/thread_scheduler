///@file: thread_lt.h
#pragma once
#include "context.h"
#include <stdint.h>

#define STACK_SIZE (1024 *64) //64kb stack

typedef enum{
    THREAD_READY,    
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_SLEEPING,
    THREAD_TERMINATED,
} lt_state;

typedef struct thread_lt
{
    int                 id;
    lt_state            state;
    uint64_t            sleep_ms;
    void                *stack;
    struct thread_lt    *next;
    struct thread_lt    *joiner; 
    context_t           ctx;
}thread_lt;

void init();
thread_lt* thread_create(void (*entry)(void*), void *arg);
void thread_exit();
void thread_yeild();
void thread_sleep(uint64_t ms);
void thread_join(thread_lt *t);