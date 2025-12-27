///@file: test_preempt.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <scheduler.h>

// Defined in your preempt.c (make sure to expose it in a header)
extern void preemption_init(int time_ms);
extern void preemption_disable();

// A CPU-bound task that refuses to yield
void greedy_worker(void *arg) {
    int id = *(int*)arg;
    long long counter = 0;
    
    printf("Worker %d: Starting infinite loop (I will NOT yield!)\n", id);
    
    while (1) {
        counter++;
        
        // Every ~50 million iterations, print a status message.
        // This simulates heavy work.
        if (counter % 50000000 == 0) {
            // NOTE: printf is not strictly signal-safe, but for this demo 
            // it usually works fine. If it crashes, increase the modulo.
            printf("[Worker %d] Still running... (Count: %lld)\n", id, counter);
        }
    }
}

int main() {
    // 1. Initialize the Scheduler
    init(); // Sets up main thread
    
    printf("=== Starting Preemption Test ===\n");
    printf("Spawning 2 greedy threads. If preemption works, they will take turns.\n");
    printf("If preemption fails, Worker 1 will run forever and Worker 2 will never start.\n");

    // 2. Create Greedy Threads
    int *id1 = malloc(sizeof(int)); *id1 = 1;
    thread_lt *t1 = thread_create(greedy_worker, id1);

    int *id2 = malloc(sizeof(int)); *id2 = 2;
    thread_lt *t2 = thread_create(greedy_worker, id2);

    // 3. Enable Preemption (Quantum = 50ms)
    preemption_init(50);

    // 4. Wait (Join)
    // Since threads are infinite, we will never actually return from here.
    // The program will run until you hit Ctrl+C.
    thread_join(t1);
    thread_join(t2);

    printf("This line should never be reached.\n");
    return 0;
}
