///@file: mutex.h
#pragma once
#include "queue.h"
#include <stdatomic.h>

// we will make this atomic int since we can extend
// this scheduler to m:n then we need atomics
typedef struct mutex{
    _Atomic int locked;
    queue wait_queue;
}mutex_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);