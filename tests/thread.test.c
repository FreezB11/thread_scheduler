///@file: thread.test.debug.c
#include <thread_lt.h>
#include <mutex.h>
#include <cond.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <scheduler.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int count = 0;
int in = 0;
int out = 0;

mutex_t mutex;
convar not_full;
convar not_empty;

void producer(void *arg) {
    int id = *(int*)arg;
    printf("[P%d] Producer started\n", id);
    
    for (int i = 0; i < 5; i++) {
        printf("[P%d] Iteration %d - trying to lock mutex\n", id, i);
        mutex_lock(&mutex);
        printf("[P%d] Iteration %d - mutex locked\n", id, i);
        
        while (count == BUFFER_SIZE) {
            printf("[P%d] Buffer full, waiting...\n", id);
            cond_wait(&not_full, &mutex);
            printf("[P%d] Woke up from full wait\n", id);
        }
        
        int item = id * 100 + i;
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("[P%d] Produced %d (Count: %d, in=%d, out=%d)\n", id, item, count, in, out);
        
        printf("[P%d] Signaling not_empty\n", id);
        cond_signal(&not_empty);
        
        printf("[P%d] Unlocking mutex\n", id);
        mutex_unlock(&mutex);
        
        printf("[P%d] Sleeping for 200ms\n", id);
        thread_sleep(200);
        printf("[P%d] Woke from sleep\n", id);
    }
    printf("[P%d] Finished.\n", id);
    free(arg);
}

void consumer(void *arg) {
    int id = *(int*)arg;
    printf("[C%d] Consumer started\n", id);
    
    for (int i = 0; i < 5; i++) {
        printf("[C%d] Iteration %d - trying to lock mutex\n", id, i);
        mutex_lock(&mutex);
        printf("[C%d] Iteration %d - mutex locked, count=%d\n", id, i, count);
        
        while (count == 0) {
            printf("[C%d] Buffer empty (count=%d), waiting...\n", id, count);
            cond_wait(&not_empty, &mutex);
            printf("[C%d] Woke up from empty wait, count=%d\n", id, count);
        }
        
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("[C%d] Consumed %d (Count: %d, in=%d, out=%d)\n", id, item, count, in, out);
        
        printf("[C%d] Signaling not_full\n", id);
        cond_signal(&not_full);
        
        printf("[C%d] Unlocking mutex\n", id);
        mutex_unlock(&mutex);
        
        printf("[C%d] Sleeping for 300ms\n", id);
        thread_sleep(300);
        printf("[C%d] Woke from sleep\n", id);
    }
    printf("[C%d] Finished.\n", id);
    free(arg);
}

int main() {
    printf("Initializing threading system...\n");
    init();
    
    printf("Initializing synchronization primitives...\n");
    mutex_init(&mutex);
    cond_init(&not_full);
    cond_init(&not_empty);

    printf("Starting Scheduler Demo (Producer-Consumer)...\n");
    printf("Main thread ID: %d\n", current_thread->id);

    int *p1_arg = malloc(sizeof(int)); *p1_arg = 1;
    thread_lt *t1 = thread_create(producer, p1_arg);
    printf("Created producer thread %d\n", t1->id);

    int *c1_arg = malloc(sizeof(int)); *c1_arg = 1;
    thread_lt *t2 = thread_create(consumer, c1_arg);
    printf("Created consumer thread %d\n", t2->id);

    printf("Main thread waiting for threads to complete...\n");
    thread_join(t1);
    printf("Producer joined\n");
    
    thread_join(t2);
    printf("Consumer joined\n");

    printf("All threads finished. Exiting.\n");
    return 0;
}