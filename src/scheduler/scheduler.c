///@file:scheduler.c
#include "../thread/thread.h"
#include "../thread/stack.h"
#include "../context/context.h"
#include <stdlib.h>
#include <stdio.h>

static uthr_thread_t *current = NULL;
static uthr_thread_t *run_queue = NULL;
static context_t scheduler_ctx;
static int thread_count = 0;

static void enqueue(uthr_thread_t *t){
    t->next = NULL;

    if(!run_queue){
        run_queue = t;
        return ;
    }

    uthr_thread_t *tmp = run_queue;
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = t;
}

static uthr_thread_t *dequeue(){
    if(!run_queue)
        return NULL;

    uthr_thread_t *t = run_queue;
    run_queue = run_queue->next;
    return t;
}

void uthr_exit(void){
    current->state = THREAD_DONE;
    context_switch(&current->context, &scheduler_ctx);
}

int uthr_create(void(*fn)(void *), void *arg){
    uthr_thread_t *t = malloc(sizeof(uthr_thread_t));
    if(!t) return -1;
    t->id = thread_count++;
    t->state = THREAD_READY;
    t->stack = malloc(STACK_SIZE);
    if(!t->stack){
        free(t);
        return -1;
    }

    void *stack_top = (uint8_t *)t->stack + STACK_SIZE;

    context_init(
        &t->context,
        fn,
        arg,
        stack_top
    );

    enqueue(t);
}