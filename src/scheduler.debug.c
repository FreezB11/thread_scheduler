///@file:scheduler_debug.c
#include <scheduler.h>
#include <utils.h>
#include <unistd.h>
#include <stdio.h>

// Global Definitions
__thread     thread_lt    *current_thread = NULL;
__thread     thread_lt    *main_thread    = NULL;
queue        ready_queue     = {NULL, NULL};
queue        sleep_queue     = {NULL, NULL};
queue        zombie_queue    = {NULL, NULL};
atomic_int   nxt_tid         = 1;

static void check_sleepers() {
    if (!sleep_queue.head) return;

    uint64_t now = current_time_ms();
    thread_lt *curr = sleep_queue.head;
    thread_lt *next_node = NULL;

    printf("[SCHED] check_sleepers: now=%lu\n", now);
    
    int count = 0;
    thread_lt *check = sleep_queue.head;
    while(check) {
        printf("[SCHED]   Thread %d: sleep_ms=%lu, ", check->id, check->sleep_ms);
        if(now >= check->sleep_ms) {
            printf("WAKING\n");
        } else {
            printf("still sleeping (need %lu more ms)\n", check->sleep_ms - now);
        }
        count++;
        check = check->next;
    }
    printf("[SCHED]   Total in sleep queue: %d\n", count);

    while(curr){
        next_node = curr->next;
        if(now >= curr->sleep_ms){
            printf("[SCHED] Waking thread %d\n", curr->id);
            queue_remove(&sleep_queue, curr);
            curr->state = THREAD_READY;
            queue_push(&ready_queue, curr);
        }
        curr = next_node;
    }
}

void schedule(){
    interrupts_disable();

    static __thread int tick = 0;
    printf("[SCHED] schedule() called, tick=%d, ready_queue.head=%p\n", 
           tick, (void*)ready_queue.head);
    
    if ((++tick & 63) == 0 || !ready_queue.head) {
        check_sleepers();
    }

    thread_lt *prev = current_thread;
    thread_lt *next = queue_pop(&ready_queue);

    printf("[SCHED] prev=%d (state=%d), next=%p\n", 
           prev ? prev->id : -1, 
           prev ? prev->state : -1,
           (void*)next);

    if(!next){
        if(sleep_queue.head){
            printf("[SCHED] No ready threads, but sleep_queue has threads. Waiting...\n");
            while(!ready_queue.head){
                check_sleepers();
                
                if(!ready_queue.head) {
                    interrupts_enable();
                    usleep(100);
                    interrupts_disable();
                }
            }
            next = queue_pop(&ready_queue);
            printf("[SCHED] Woke up thread %d\n", next ? next->id : -1);
        } else {
            if(prev && prev->state == THREAD_TERMINATED) {
                printf("[SCHED] All done, prev terminated\n");
                interrupts_enable();
                return;
            }
        }
    }

    while (zombie_queue.head) {
        thread_lt *z = queue_pop(&zombie_queue);
        printf("[SCHED] Cleaning up zombie thread %d\n", z->id);
        free(z->stack);
        free(z);
    }

    if(next && next != prev){
        printf("[SCHED] Switching from thread %d to thread %d\n", 
               prev ? prev->id : -1, next->id);
        
        current_thread = next;
        current_thread->state = THREAD_RUNNING;

        if(prev && prev->state == THREAD_RUNNING){
            printf("[SCHED] prev %d still RUNNING, moving to ready\n", prev->id);
            prev->state = THREAD_READY;
            queue_push(&ready_queue, prev);
        } else if(prev) {
            printf("[SCHED] prev %d in state %d, not adding to ready\n", 
                   prev->id, prev->state);
        }

        context_switch(&prev->ctx, &current_thread->ctx);
        printf("[SCHED] Returned to thread %d\n", current_thread->id);
    } else {
        printf("[SCHED] No context switch needed\n");
    }

    interrupts_enable();
}