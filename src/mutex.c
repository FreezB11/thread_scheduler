///@file: mutex.c
#include <mutex.h>
#include <scheduler.h>
#include <stdlib.h>
#include <stdatomic.h>
// we have interupts
#include <utils.h>

/*
    i am not sure with atomics but 
    this is what i understood so yea
*/

void mutex_init(mutex_t *m){
    // m->locked = 0;
    atomic_store(&m->locked, 0);

    m->wait_queue.head = m->wait_queue.tail = NULL;
}

// void mutex_lock(mutex_t *m){
//     if(atomic_load(&m->locked) == 0){
//         atomic_store(&m->locked ,1);
//     }else{
//         // block current thread
//         current_thread->state = THREAD_BLOCKED;
//         queue_push(&m->wait_queue, current_thread);
//         schedule();
//     }
// }

void mutex_lock(mutex_t *m){
    // atomic test && set
    if(atomic_exchange_explicit(&m->locked, 1, memory_order_acquire) == 0){
        return; // acquired the lock
    }
    // if failed we will sleep
    // we will disable the premption here;
    // we have the spinlock too maybe that can work
    interrupts_disable();

    //but 
    if(atomic_exchange_explicit(&m->locked, 1, memory_order_acquire) == 0){
        // we will enable the premption here
        interrupts_enable();
        return; // acquired the lock
    }
    current_thread->state = THREAD_BLOCKED;
    queue_push(&m->wait_queue, current_thread);

    //premption enable
    interrupts_enable();
    //schedule
    schedule();
}

void mutex_unlock(mutex_t *m){
    // disable preempt
    interrupts_disable();

    if(m->wait_queue.head){
        thread_lt *t = queue_pop(&m->wait_queue);
        t->state = THREAD_READY;
        queue_push(&ready_queue, t);
        //lock is held by the thread(awake)
    }else{
        // we will let go of the lock
        atomic_store_explicit(&m->locked,0,memory_order_release);
    }

    interrupts_enable();
}