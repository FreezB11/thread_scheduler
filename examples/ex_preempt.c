///@file: ex_preempt.c
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep() in main

// You need to declare these if they aren't in thread_lt.h yet
extern void preemption_init(int time_ms);
extern void preemption_disable();

// Global flag to stop threads eventually
volatile int keep_running = 1;

void cpu_hog(void *arg) {
    int id = *(int*)arg;
    long long counter = 0;
    
    printf("Worker %d: Starting infinite loop (I will NOT yield!)\n", id);
    
    while (keep_running) {
        counter++;
        // Heavy work... no thread_yield() call here!
        
        // Just print occasionally so we know who is running
        // Using a large modulo to simulate heavy CPU usage between checks
        if (counter % 50000000 == 0) {
            printf("[Worker %d] Still running... (Count: %lld)\n", id, counter);
        }
    }
    printf("Worker %d: Stopped.\n", id);
    free(arg);
}

int main() {
    init();

    printf("--- Preemption Demo ---\n");
    printf("Spawning 2 greedy threads. If preemption works, they will take turns.\n");
    printf("If preemption fails, Worker 1 will run forever and Worker 2 will never start.\n");

    // 1. Create Greedy Threads
    int *id1 = malloc(sizeof(int)); *id1 = 1;
    thread_lt *t1 = thread_create(cpu_hog, id1);

    int *id2 = malloc(sizeof(int)); *id2 = 2;
    thread_lt *t2 = thread_create(cpu_hog, id2);

    // 2. Enable Preemption (Quantum = 50ms)
    // This is the magic line. The OS will interrupt the loops every 50ms.
    preemption_init(50);

    // 3. Let them run for 3 seconds
    // Note: main thread is sleeping, but the SIGALRM fires in the process context,
    // causing the *currently running thread* (t1 or t2) to yield.
    // Since main is blocked in sleep/join, it won't be scheduled until it wakes.
    
    // We use a busy loop in main just to keep the process alive while watching output
    // or just sleep.
    // Ideally, we join. But we need to tell them to stop first.
    
    // Let's just sleep for 3 seconds to observe the switching
    // NOTE: 'sleep' might interact with SIGALRM on some old systems, 
    // but on Linux with SA_RESTART it's usually fine.
    // Safer way: busy wait for 3 seconds in main or just use a loop.
    
    unsigned long long start = 0;
    while(start < 400000000) { start++; } // Busy wait ~1-2 sec

    printf("\n>>> TIME UP! Stopping threads... <<<\n");
    keep_running = 0; // Tell threads to exit

    thread_join(t1);
    thread_join(t2);

    preemption_disable();
    printf("--- Done ---\n");
    return 0;
}
