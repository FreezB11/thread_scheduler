///@file: thread.test.c
#include <thread_lt.h>
#include <mutex.h>
#include <cond.h>
#include <stdio.h>
#include <stdlib.h>

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
    for (int i = 0; i < 5; i++) {
        mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            printf("[P%d] Buffer full, waiting...\n", id);
            cond_wait(&not_full, &mutex);
        }
        
        int item = id * 100 + i;
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("[P%d] Produced %d (Count: %d)\n", id, item, count);
        
        cond_signal(&not_empty);
        mutex_unlock(&mutex);
        
        thread_sleep(200); // Simulate work
    }
    printf("[P%d] Finished.\n", id);
    free(arg);
}

void consumer(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        mutex_lock(&mutex);
        while (count == 0) {
            printf("[C%d] Buffer empty, waiting...\n", id);
            cond_wait(&not_empty, &mutex);
        }
        
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("[C%d] Consumed %d (Count: %d)\n", id, item, count);
        
        cond_signal(&not_full);
        mutex_unlock(&mutex);
        
        thread_sleep(300); // Simulate processing
    }
    printf("[C%d] Finished.\n", id);
    free(arg);
}

int main() {
    init();
    mutex_init(&mutex);
    cond_init(&not_full);
    cond_init(&not_empty);

    printf("Starting Scheduler Demo (Producer-Consumer)...\n");

    int *p1_arg = malloc(sizeof(int)); *p1_arg = 1;
    thread_lt *t1 = thread_create(producer, p1_arg);

    int *c1_arg = malloc(sizeof(int)); *c1_arg = 1;
    thread_lt *t2 = thread_create(consumer, c1_arg);
    
    // int *p2_arg = malloc(sizeof(int)); *p2_arg = 2;
    // thread_lt *t3 = thread_create(producer, p2_arg);

    // Main thread acts as a supervisor or simply waits
    thread_join(t1);
    thread_join(t2);
    // thread_join(t3);

    printf("All threads finished. Exiting.\n");
    return 0;
}