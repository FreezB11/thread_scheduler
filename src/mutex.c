///@file: mutex.c
#include <mutex.h>
#include <scheduler.h>
#include <stdlib.h>

void mutex_init(mutex_t *m){
    m->locked = 0;
    m->wait_queue.head = m->wait_queue.tail = NULL;
}

void mutex_lock(mutex_t *m){
    if(m->locked == 0){
        m->locked = 1;
    }else{
        // block current thread
        current_thread->state = THREAD_BLOCKED;
        queue_push(&m->wait_queue, current_thread);
        schedule();
    }
}

void mutex_unlock(mutex_t *m){
    if(m->wait_queue.head){
        thread_lt *t = queue_pop(&m->wait_queue);
        t->state = THREAD_READY;
        queue_push(&ready_queue, t);
    }else{
        m->locked  =0;
    }
}