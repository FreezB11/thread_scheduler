///@file:thread.c
#include <queue.h>
#include <thread_lt.h>
#include <context.h>
#include <scheduler.h>
#include <utils.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void thread_yield(){
    schedule();
}

void thread_sleep(uint64_t ms){
    current_thread->state = THREAD_SLEEPING;
    current_thread->sleep_ms = current_time_ms() + ms;
    queue_push_sorted(&sleep_queue, current_thread);
    // queue_push(&sleep_queue, current_thread);
    schedule();
}

void thread_exit(){
    current_thread->state = THREAD_TERMINATED;

    //wake up joiner if any
    if(current_thread->joiner){
        current_thread->joiner->state = THREAD_READY;
        queue_push(&ready_queue, current_thread->joiner);
        current_thread->joiner = NULL;
    }

    queue_push(&zombie_queue, current_thread);
    schedule();
    __builtin_unreachable();
}

// // __attribute__((noreturn))
// void thread_trampoline(void) {
//     void (*fn)(void *);
//     void *arg;

//     __asm__ volatile (
//         "mov %%r12, %0\n"
//         "mov %%r13, %1\n"
//         : "=r"(fn), "=r"(arg)
//     );

//     fn(arg);
//     thread_exit();
// }

thread_lt* thread_create(void (*fn)(void*), void *arg){
    // printf("creating thread\n");
    thread_lt *t = (thread_lt*)malloc(sizeof(thread_lt));
    // t->id = nxt_tid++;
    t->id = atomic_fetch_add(&nxt_tid, 1);
    t->state = THREAD_READY;

    t->joiner = NULL;
    t->stack = malloc(STACK_SIZE);
    if(!t->stack){
        free(t);
        return NULL;
    }
    void *stack_top = (uint8_t *)t->stack + STACK_SIZE;

    context_init(
        &t->ctx,
        fn, arg,
        stack_top
    );
    // uint8_t *stack_top = (uint8_t *)t->stack + STACK_SIZE;
    // stack_top = (uint8_t *)((uintptr_t)stack_top & ~0xF);

    // stack_top -= sizeof(void *);
    // *(void **)stack_top = fn;

    // t->ctx->rsp = (uint64_t)stack_top;
    // t->ctx->rip = (uint64_t)thread_trampoline;

    queue_push(&ready_queue, t);
    return t;
}

void thread_join(thread_lt *t){
    // printf("we will be joining some threads\n");
    if(t->state == THREAD_TERMINATED) return;
    current_thread->state = THREAD_BLOCKED;
    t->joiner = current_thread;
    schedule();
}

void init(){
    // convert the current execution context into the Main thread
    main_thread = (thread_lt*)malloc(sizeof(thread_lt));
    main_thread->id = 0;
    main_thread->state = THREAD_RUNNING;
    // we dont know the stack base of main sett NULL
    main_thread->stack = NULL;
    current_thread = main_thread;
}