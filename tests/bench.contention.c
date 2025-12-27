///@file: bench_contention.c
#define _POSIX_C_SOURCE 200809L
#include <thread_lt.h>
#include <mutex.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_FIGHTERS 100
#define OPS_PER_FIGHTER 1000

mutex_t global_lock;
long long shared_counter = 0;

void fighter(void *arg) {
    int id = *(int*)arg;
    for(int i=0; i<OPS_PER_FIGHTER; i++) {
        mutex_lock(&global_lock);
        shared_counter++; // Critical Section
        mutex_unlock(&global_lock);
        
        // Optional: Yield to increase contention chaos
        // thread_yield(); 
    }
    free(arg);
}

int main() {
    init();
    mutex_init(&global_lock);
    
    printf("=== BENCHMARK: Mutex Contention ===\n");
    printf("Threads: %d, Ops/Thread: %d\n", NUM_FIGHTERS, OPS_PER_FIGHTER);
    
    thread_lt *pool[NUM_FIGHTERS];
    
    for(int i=0; i<NUM_FIGHTERS; i++) {
        int *id = malloc(sizeof(int)); *id = i;
        pool[i] = thread_create(fighter, id);
    }
    
    for(int i=0; i<NUM_FIGHTERS; i++) {
        thread_join(pool[i]);
    }
    
    printf("Expected: %d\n", NUM_FIGHTERS * OPS_PER_FIGHTER);
    printf("Actual:   %lld\n", shared_counter);
    
    if (shared_counter == NUM_FIGHTERS * OPS_PER_FIGHTER)
        printf("SUCCESS: No Race Conditions.\n");
    else
        printf("FAIL: Race Condition Detected!\n");
        
    return 0;
}
