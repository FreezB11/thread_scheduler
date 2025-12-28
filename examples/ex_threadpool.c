///@file: ex_threadpool.c
#include <thread_lt.h>
#include <mutex.h>
#include <cond.h>
#include <stdio.h>
#include <stdlib.h>

#define POOL_SIZE 3
#define NUM_TASKS 10

// Task Definition
typedef struct task {
    int id;
    struct task *next;
} task_t;

// Shared Work Queue
task_t *task_head = NULL;
mutex_t q_mtx;
convar q_cv;
int stop_pool = 0;

void worker(void *arg) {
    int id = *(int*)arg;
    printf("Worker %d started.\n", id);

    while(1) {
        mutex_lock(&q_mtx);
        
        // Wait for work
        while (task_head == NULL && !stop_pool) {
            cond_wait(&q_cv, &q_mtx);
        }

        if (stop_pool && task_head == NULL) {
            mutex_unlock(&q_mtx);
            break; // Exit
        }

        // Grab task
        task_t *job = task_head;
        task_head = job->next;
        mutex_unlock(&q_mtx);

        // Process task (outside lock)
        printf("Worker %d executing Task %d...\n", id, job->id);
        thread_sleep(50); // Simulate work
        free(job);
    }
    printf("Worker %d stopping.\n", id);
    free(arg);
}

int main() {
    init();
    mutex_init(&q_mtx);
    cond_init(&q_cv);

    // 1. Start Workers
    thread_lt *pool[POOL_SIZE];
    for(int i=0; i<POOL_SIZE; i++) {
        int *id = malloc(sizeof(int)); *id = i;
        pool[i] = thread_create(worker, id);
    }

    // 2. Submit Tasks
    for(int i=0; i<NUM_TASKS; i++) {
        mutex_lock(&q_mtx);
        task_t *t = malloc(sizeof(task_t));
        t->id = i;
        t->next = task_head;
        task_head = t;
        printf("[Main] Submitted Task %d\n", i);
        cond_signal(&q_cv); // Wake one worker
        mutex_unlock(&q_mtx);
        
        thread_sleep(20); // Simulate submission delay
    }

    // 3. Shutdown Signal
    mutex_lock(&q_mtx);
    stop_pool = 1;
    cond_broadcast(&q_cv); // Wake everyone to check stop flag
    mutex_unlock(&q_mtx);

    // 4. Join
    for(int i=0; i<POOL_SIZE; i++) thread_join(pool[i]);

    printf("All tasks completed.\n");
    return 0;
}
