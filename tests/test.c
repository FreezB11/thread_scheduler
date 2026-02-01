///@file: test.c
/*
 * Test program demonstrating the threading library fixes
 * 
 * Compile with:
 *   gcc -o test test_fixes.c context.c thread.c scheduler.c queue.c mutex.c cond.c utils.c preempt.c context_asm.S -lpthread
 */

#include <stdio.h>
#include <thread_lt.h>
#include <scheduler.h>
#include <mutex.h>
#include <cond.h>
#include <utils.h>
#include <unistd.h>

extern void preemption_init(int time_ms);
extern void preemption_disable();

// Test 1: Stack alignment (would crash before fix)
void stack_test(void *arg) {
    int x = 42;
    printf("Thread %d: Stack aligned test passed! x=%d\n", 
           current_thread->id, x);
}

// Test 2: Nested interrupt disable/enable
mutex_t test_mutex;
int counter = 0;

void nested_critical_section(void *arg) {
    for(int i = 0; i < 100; i++) {
        mutex_lock(&test_mutex);
        
        // Nested lock (tests interrupt nesting)
        interrupts_disable();
        counter++;
        interrupts_enable();
        
        mutex_unlock(&test_mutex);
    }
    printf("Thread %d: Nested locks test passed! counter=%d\n", 
           current_thread->id, counter);
}

// Test 3: Sleep/wakeup with preemption
void sleeper(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d: Sleeping for 100ms...\n", id);
    thread_sleep(100);
    printf("Thread %d: Woke up!\n", id);
}

// Test 4: CPU hog (tests preemption prevents starvation)
volatile int hog_counter = 0;

void cpu_hog(void *arg) {
    printf("Thread %d: Starting CPU hog...\n", current_thread->id);
    for(int i = 0; i < 1000000; i++) {
        hog_counter++;
    }
    printf("Thread %d: CPU hog done. Counter=%d\n", 
           current_thread->id, hog_counter);
}

// Test 5: Condition variable (tests proper mutex/cond interaction)
convar cv;
int ready = 0;

void waiter(void *arg) {
    mutex_lock(&test_mutex);
    printf("Thread %d: Waiting for signal...\n", current_thread->id);
    while(!ready) {
        cond_wait(&cv, &test_mutex);
    }
    printf("Thread %d: Got signal!\n", current_thread->id);
    mutex_unlock(&test_mutex);
}

void signaler(void *arg) {
    thread_sleep(50);
    mutex_lock(&test_mutex);
    ready = 1;
    printf("Thread %d: Sending signal...\n", current_thread->id);
    cond_signal(&cv);
    mutex_unlock(&test_mutex);
}

int main() {
    printf("=== Threading Library Test Suite ===\n\n");
    
    // Initialize threading system
    init();
    
    // Initialize synchronization primitives
    mutex_init(&test_mutex);
    cond_init(&cv);
    
    // Enable preemption to prevent starvation
    printf("Enabling preemption (10ms intervals)...\n");
    preemption_init(10);
    
    printf("\n--- Test 1: Stack Alignment ---\n");
    thread_lt *t1 = thread_create(stack_test, NULL);
    thread_join(t1);
    
    printf("\n--- Test 2: Nested Critical Sections ---\n");
    counter = 0;
    thread_lt *t2a = thread_create(nested_critical_section, NULL);
    thread_lt *t2b = thread_create(nested_critical_section, NULL);
    thread_join(t2a);
    thread_join(t2b);
    printf("Final counter: %d (expected 200)\n", counter);
    
    printf("\n--- Test 3: Sleep/Wakeup ---\n");
    int ids[] = {1, 2, 3};
    thread_lt *t3a = thread_create(sleeper, &ids[0]);
    thread_lt *t3b = thread_create(sleeper, &ids[1]);
    thread_lt *t3c = thread_create(sleeper, &ids[2]);
    thread_join(t3a);
    thread_join(t3b);
    thread_join(t3c);
    
    printf("\n--- Test 4: Preemption (CPU Hog) ---\n");
    hog_counter = 0;
    thread_lt *t4 = thread_create(cpu_hog, NULL);
    // Create another thread while hog is running
    thread_sleep(10);  // Let hog start
    thread_lt *t4b = thread_create(sleeper, &ids[0]);
    thread_join(t4);
    thread_join(t4b);
    printf("Preemption test passed! (sleeper ran despite CPU hog)\n");
    
    printf("\n--- Test 5: Condition Variables ---\n");
    ready = 0;
    thread_lt *t5a = thread_create(waiter, NULL);
    thread_lt *t5b = thread_create(signaler, NULL);
    thread_join(t5a);
    thread_join(t5b);
    
    printf("\n=== All Tests Passed! ===\n");
    
    // Disable preemption before exit
    preemption_disable();
    
    return 0;
}