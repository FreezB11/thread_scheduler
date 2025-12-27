///@file: mutex.h
#pragma once
#include "queue.h"

typedef struct mutex{
    int locked;
    queue wait_queue;
}mutex_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);