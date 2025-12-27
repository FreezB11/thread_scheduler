///@file:cond.c
#include <cond.h>
#include <mutex.h>
#include <scheduler.h>
#include <queue.h>
#include <stdlib.h>

void cond_init(convar *c){
    c->wait_queue.head = c->wait_queue.tail = NULL;
}

void cond_wait(convar *c, mutex_t *m){
    mutex_unlock(m);
    current_thread->state = THREAD_BLOCKED;
    queue_push(&c->wait_queue, current_thread);
    schedule();
    mutex_lock(m);
}

void cond_signal(convar *c){
    if(c->wait_queue.head){
        thread_lt *t = queue_pop(&c->wait_queue);
        t->state = THREAD_READY;
        queue_push(&ready_queue, t);
    }
}

void cond_broadcast(convar *c){
    while(c->wait_queue.head) cond_signal(c);
}