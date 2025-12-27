///@file: bench_churn.c
#define _POSIX_C_SOURCE 200809L
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_OPS 50000
#define MAX_ACTIVE 100

thread_lt *pool[MAX_ACTIVE];

void short_task(void *arg) {
    int loops = rand() % 100;
    for(int i=0; i<loops; i++) thread_yield();
}

int main() {
    init();
    srand(time(NULL));
    printf("=== BENCHMARK: Thread Churn (Spawn/Join) ===\n");

    for(int i=0; i<MAX_ACTIVE; i++) pool[i] = NULL;

    int spawned = 0;
    int completed = 0;

    for(int i=0; i<TOTAL_OPS; i++) {
        int slot = rand() % MAX_ACTIVE;
        
        if (pool[slot] == NULL) {
            // Spawn
            pool[slot] = thread_create(short_task, NULL);
            spawned++;
        } else {
            // Join (Kill)
            thread_join(pool[slot]);
            pool[slot] = NULL;
            completed++;
        }
        
        if (i % 1000 == 0) {
            printf("\rOps: %d | Active: %d | Zombies Freed: %d", i, spawned-completed, completed);
            fflush(stdout);
        }
    }

    // Cleanup remaining
    for(int i=0; i<MAX_ACTIVE; i++) {
        if (pool[i]) thread_join(pool[i]);
    }

    printf("\nDone. %d threads created and destroyed.\n", spawned);
    return 0;
}
// core dump dont use