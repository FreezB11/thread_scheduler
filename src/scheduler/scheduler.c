///@file:scheduler.c
#define _GNU_SOURCE
#include "../thread/thread.h"
#include "../thread/stack.h"
#include "../context/context.h"
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static uthr_thread_t *current = NULL;
static uthr_thread_t *run_queue = NULL;
static context_t scheduler_ctx;

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

/*
we will use alarm_mask to block SIGALRM during scheduling
*/
static sigset_t alarm_mask;
// preemption pt
static void timer_handler(int sig){
    (void)sig;
    if(!current || current->state != THREAD_RUNNING) return;

    current->state = THREAD_READY;
    enqueue(current);

    context_switch(&current->context, &scheduler_ctx);
}

// timer-installation
static void timer_start(){
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000; // 10ms time slice
    timer.it_value = timer.it_interval;

    setitimer(ITIMER_REAL, &timer, NULL);
}

static int thread_count = 0;

// __attribute__((noreturn))
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

    return t->id;
}

// was required in v1
// void uthr_yield(void){
//     current->state = THREAD_READY;
//     enqueue(current);
//     context_switch(&current->context, &scheduler_ctx);
// }

static void scheduler_lock(){
    sigprocmask(SIG_BLOCK, &alarm_mask, NULL);
}

static void scheduler_unlock(){
    sigprocmask(SIG_UNBLOCK, &alarm_mask, NULL);
}
// optional now
void uthr_yield(void){
    scheduler_lock();
    current->state = THREAD_READY;
    enqueue(current);
    scheduler_unlock();

    context_switch(&current->context, &scheduler_ctx);
}

static void scheduler_init(){
    sigemptyset(&alarm_mask);
    sigaddset(&alarm_mask, SIGALRM);
    timer_start();
}

// v1
// void scheduler_run(){
//     context_switch(&scheduler_ctx, &scheduler_ctx);

//     while((current = dequeue()) != NULL){
//         if(current->state == THREAD_DONE){
//             free(current->stack);
//             free(current);
//             continue;
//         }

//         current->state = THREAD_RUNNING;
//         context_switch(&scheduler_ctx, &current->context);
//     }
// }

// v1.5
void scheduler_run(){
    scheduler_init();

    while(1){
        scheduler_lock();
        current = dequeue();
        scheduler_unlock();

        if(!current) break;

        if(current->state == THREAD_DONE){
            free(current->stack);
            free(current);
            continue;
        }

        current->state = THREAD_RUNNING;
        context_switch(&scheduler_ctx, &current->context);
    }
}