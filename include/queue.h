///@file: queue.h
#pragma once
#include "thread_lt.h"

typedef struct{
    thread_lt   *head;
    thread_lt   *tail;
}queue;

void queue_push(queue *q, thread_lt *t);
thread_lt* queue_pop(queue* q);
void queue_remove(queue *q, thread_lt *target);

void queue_push_sorted(queue *q, thread_lt *t);