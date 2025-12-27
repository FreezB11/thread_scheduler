///@file:cond.h
#pragma once
#include "queue.h"
#include "mutex.h"

typedef struct{
    queue wait_queue;
}convar;

void cond_init(convar *c);
void cond_wait(convar *c, mutex_t *m);
void cond_signal(convar *c);
void cond_broadcast(convar *c);