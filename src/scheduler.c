///@file:scheduler.c
#include <scheduler.h>
#include <utils.h> // for interrupts_disable/enable
#include <unistd.h>
#include <stdio.h>

// Global Definitions
__thread     thread_lt    *current_thread = NULL;
__thread     thread_lt    *main_thread    = NULL;
queue        ready_queue     = {NULL, NULL};
queue        sleep_queue     = {NULL, NULL};
queue        zombie_queue    = {NULL, NULL};
atomic_int   nxt_tid         = 1;

// Helper to process sleep queue (extracted for clarity)
static void check_sleepers() {
    if (!sleep_queue.head) return;

    uint64_t now = current_time_ms();
    thread_lt *curr = sleep_queue.head;
    thread_lt *next_node = NULL;

    while(curr){
        next_node = curr->next;
        if(now >= curr->sleep_ms){
            queue_remove(&sleep_queue, curr);
            curr->state = THREAD_READY;
            queue_push(&ready_queue, curr);
        }
        curr = next_node;
    }
}

void schedule(){
    interrupts_disable();

    // 1. OPTIMIZATION: Check sleepers lazily
    // 'tick' avoids calling expensive current_time_ms() every switch.
    // We check if:
    //  a) We hit 64 ticks (periodic check)
    //  b) Ready queue is empty (nothing else to do, might as well check)
    static __thread int tick = 0;
    if ((++tick & 63) == 0 || !ready_queue.head) {
        check_sleepers();
    }

    // 2. Select next thread
    thread_lt *prev = current_thread;
    thread_lt *next = queue_pop(&ready_queue);

    // 3. Handle Idle / No Work
    if(!next){
        // If sleep queue has items, we must wait for them.
        if(sleep_queue.head){
            // Force busy wait loop
            while(!ready_queue.head){
                check_sleepers(); // Keep checking
                
                // If still empty, sleep a bit to save CPU
                if(!ready_queue.head) {
                    interrupts_enable(); // Must enable to let signals/timers work!
                    usleep(100); 
                    interrupts_disable();
                }
            }
            next = queue_pop(&ready_queue);
        } else {
            // No sleepers, no ready threads.
            // If prev is terminated, we are done.
            if(prev && prev->state == THREAD_TERMINATED) {
                 interrupts_enable();
                 return;
            }
            // If prev is blocked (e.g. main thread waiting) and nothing else exists -> Deadlock?
            // Or maybe just return to main (if main is current).
        }
    }

    // 4. Zombie Cleanup
    // Optimization: Don't do this every single switch if queue is empty.
    while (zombie_queue.head) {
        thread_lt *z = queue_pop(&zombie_queue);
        free(z->stack);
        free(z);
    }

    // 5. Context Switch
    if(next && next != prev){
        current_thread = next;
        current_thread->state = THREAD_RUNNING;

        if(prev->state == THREAD_RUNNING){
            prev->state = THREAD_READY;
            queue_push(&ready_queue, prev);
        }

        // context_switch(&prev->ctx, &current_thread->ctx);
        context_switch(&prev->ctx, &next->ctx);
    }

    interrupts_enable();
}
