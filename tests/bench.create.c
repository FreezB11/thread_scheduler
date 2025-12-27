///@file: bench_create.c
#define _POSIX_C_SOURCE 200809L
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS 100000

void dummy_worker(void *arg) {
    (void)arg;
    // Do nothing and exit immediately
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    init();
    printf("=== BENCHMARK: Thread Creation ===\n");
    printf("Spawning %d threads...\n", NUM_THREADS);

    thread_lt **threads = malloc(sizeof(thread_lt*) * NUM_THREADS);
    
    double start = get_time();
    
    for(int i=0; i<NUM_THREADS; i++) {
        threads[i] = thread_create(dummy_worker, NULL);
        if(!threads[i]) {
            printf("FATAL: OOM at thread %d\n", i);
            exit(1);
        }
    }
    
    double end = get_time();
    printf("Created %d threads in %.4fs\n", NUM_THREADS, end - start);
    printf("Rate: %.0f threads/sec\n", NUM_THREADS / (end - start));

    printf("Cleaning up (Joining all)...\n");
    // We must join them to free their stacks (since your scheduler frees on join/schedule)
    for(int i=0; i<NUM_THREADS; i++) {
        thread_join(threads[i]);
    }
    
    printf("Done.\n");
    return 0;
}