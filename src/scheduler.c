///@file:scheduler.c
#include <scheduler.h>
#include <utils.h>
#include <unistd.h>
#include <stdio.h>

thread_lt    *current_thread = NULL;
thread_lt    *main_thread    = NULL;
queue        ready_queue     = {NULL, NULL};
queue        sleep_queue     = {NULL, NULL};
queue        zombie_queue    = {NULL, NULL};
int          nxt_tid         = 1;

void schedule(){
    // printf("we have hit the scheduler\n");
    // check for waking sleepers
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
    // select nxt thread
    thread_lt *prev = current_thread;
    thread_lt *next = queue_pop(&ready_queue);
    // printf("the next thread will be %p\n", next);

    if(!next){
        //no threads ready, if sleeps exist , idle loop
        // if no sleepers and no ready, deadlock or finish
        if(sleep_queue.head){
            // simple busy wait
            while(!ready_queue.head){
                now = current_time_ms();
                curr = sleep_queue.head;
                while(curr){
                    next_node = curr->next;
                    if(now >= curr->sleep_ms){
                        queue_remove(&sleep_queue, curr);
                        curr->state = THREAD_READY;
                        queue_push(&ready_queue, curr);
                    }
                    curr = next_node;
                }
                if(!ready_queue.head) usleep(100); // prevent 100% cpu
            }
            next = queue_pop(&ready_queue);
        }else{
            // only main thread left? or deadlock??
            // if prec is terminated and no else is ready we are done.
            if(prev && prev->state == THREAD_TERMINATED) return;
            // return to main execution if main
        }
    }
    // clean up the zombies(safe to free stacks now that we are off them)
    while (zombie_queue.head)
    {
        // printf("found some zombie threads so will free them\n");
        thread_lt *z = queue_pop(&zombie_queue);
        free(z->stack);
        free(z);
    }
    // switch context
    if(next && next != prev){
        current_thread = next;
        current_thread->state = THREAD_RUNNING;

        // if prev is stilll valid (not exited/blocking impees we round-robin
        // or yield it), saveit
        if(prev->state == THREAD_RUNNING){
            prev->state = THREAD_READY;
            queue_push(&ready_queue, prev);
        }
        // printf("we will be context switching here\n");
        context_switch(&prev->ctx, &current_thread->ctx);
    }
}