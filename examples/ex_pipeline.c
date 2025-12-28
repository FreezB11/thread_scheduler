///@file: ex_pipeline.c
#include <thread_lt.h>
#include <mutex.h>
#include <cond.h>
#include <stdio.h>
#include <stdlib.h>

int shared_data = -1;
int data_ready = 0; // Predicate
int done = 0;

mutex_t mtx;
convar cv_data;

void generator(void *arg) {
    (void)arg;
    for(int i=1; i<=5; i++) {
        mutex_lock(&mtx);
        shared_data = i * 10;
        data_ready = 1;
        printf("[Gen] Generated: %d\n", shared_data);
        cond_signal(&cv_data);
        mutex_unlock(&mtx);
        
        thread_sleep(100); // Simulate generation time
    }
    
    mutex_lock(&mtx);
    done = 1;
    cond_signal(&cv_data); // Wake up processor one last time
    mutex_unlock(&mtx);
}

void processor(void *arg) {
    (void)arg;
    while(1) {
        mutex_lock(&mtx);
        while(!data_ready && !done) {
            cond_wait(&cv_data, &mtx);
        }
        
        if (done && !data_ready) {
            mutex_unlock(&mtx);
            break;
        }

        printf("  [Proc] Processed: %d\n", shared_data);
        data_ready = 0; // Reset
        mutex_unlock(&mtx);
    }
    printf("  [Proc] Exiting.\n");
}

int main() {
    init();
    mutex_init(&mtx);
    cond_init(&cv_data);

    thread_lt *p = thread_create(processor, NULL);
    thread_lt *g = thread_create(generator, NULL);

    thread_join(g);
    thread_join(p);
    return 0;
}
