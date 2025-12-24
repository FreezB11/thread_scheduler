/// @file: thread.h
#pragma once
#include "../context/context.h"
#include "../../include/uthr.h"

typedef enum{
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_DONE
}thread_state;

typedef struct uthr_thread{
    int             id;
    thread_state    state;
    context_t       context;
    void            *stack;
    struct uthr_thread   *next;
}uthr_thread_t;