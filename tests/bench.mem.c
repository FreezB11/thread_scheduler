///@file: bench_memory.c
#define _POSIX_C_SOURCE 200809L
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sleeper_worker(void *arg) {
    // Just sleep forever so we keep the stack allocated
    while(1) thread_yield();
}

int main() {
    init();
    printf("=== BENCHMARK: Max Capacity Test ===\n");
    printf("Stack Size per thread: 64KB (approx)\n");

    int count = 0;
    while(1) {
        thread_lt *t = thread_create(sleeper_worker, NULL);
        if (!t) {
            printf("\nHit Limit! Could not create thread #%d\n", count);
            break;
        }
        count++;
        if (count % 1000 == 0) {
            printf("\rActive Threads: %d (%d MB Used)", count, (count * 64) / 1024);
            fflush(stdout);
        }
        
        // Safety Break (Don't freeze your laptop)
        if (count >= 200000) { 
            printf("\nStopping at 200k threads safety limit.\n");
            break; 
        }
    }
    
    printf("\nFinal Count: %d threads alive.\n", count);
    return 0;
}
