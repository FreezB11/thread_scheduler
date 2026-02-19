///@file:thread_debug.c
#include <queue.h>
#include <thread_lt.h>
#include <context.h>
#include <scheduler.h>
#include <utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void thread_yield(){
    printf("[THREAD %d] Yielding\n", current_thread->id);
    schedule();
}

void thread_sleep(uint64_t ms){
    uint64_t now = current_time_ms();
    uint64_t wake = now + ms;
    
    printf("[THREAD %d] thread_sleep(%lu): now=%lu, wake=%lu\n", 
           current_thread->id, ms, now, wake);
    
    current_thread->state = THREAD_SLEEPING;
    current_thread->sleep_ms = wake;
    
    printf("[THREAD %d] Adding to sleep_queue\n", current_thread->id);
    queue_push(&sleep_queue, current_thread);
    
    printf("[THREAD %d] Calling schedule()\n", current_thread->id);
    schedule();
    
    printf("[THREAD %d] Woke up from sleep\n", current_thread->id);
}

void thread_exit(){
    printf("[THREAD %d] Exiting\n", current_thread->id);
    fflush(stdout);
    
    current_thread->state = THREAD_TERMINATED;

    if(current_thread->joiner){
        printf("[THREAD %d] Waking joiner %d\n", 
               current_thread->id, current_thread->joiner->id);
        current_thread->joiner->state = THREAD_READY;
        queue_push(&ready_queue, current_thread->joiner);
        current_thread->joiner = NULL;
    }

    queue_push(&zombie_queue, current_thread);
    schedule();
    
    __builtin_unreachable();
}

thread_lt* thread_create(void (*fn)(void*), void *arg){
    thread_lt *t = (thread_lt*)malloc(sizeof(thread_lt));
    if (!t) return NULL;
    
    t->id = atomic_fetch_add(&nxt_tid, 1);
    t->state = THREAD_READY;
    t->joiner = NULL;
    
    printf("[THREAD CREATE] Creating thread %d\n", t->id);
    
    t->stack = malloc(STACK_SIZE);
    if(!t->stack){
        free(t);
        return NULL;
    }
    
    void *stack_top = (uint8_t *)t->stack + STACK_SIZE;
    context_init(&t->ctx, fn, arg, stack_top);
    queue_push(&ready_queue, t);
    
    return t;
}

void thread_join(thread_lt *t){
    printf("[THREAD %d] Joining thread %d (state=%d)\n", 
           current_thread->id, t->id, t->state);
    
    if(t->state == THREAD_TERMINATED) {
        printf("[THREAD %d] Thread %d already terminated\n", 
               current_thread->id, t->id);
        return;
    }
    
    current_thread->state = THREAD_BLOCKED;
    t->joiner = current_thread;
    schedule();
    
    printf("[THREAD %d] Join complete for thread %d\n", 
           current_thread->id, t->id);
}

void init(){
    main_thread = (thread_lt*)malloc(sizeof(thread_lt));
    main_thread->id = 0;
    main_thread->state = THREAD_RUNNING;
    main_thread->stack = NULL;
    main_thread->joiner = NULL;
    
    current_thread = main_thread;
    printf("[INIT] Main thread initialized as thread %d\n", main_thread->id);
}