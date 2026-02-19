///@file: test_diagnostic.c
#include <stdio.h>
#include <thread_lt.h>
#include <scheduler.h>
#include <mutex.h>
#include <cond.h>
#include <utils.h>
#include <unistd.h>

extern void preemption_init(int time_ms);
extern void preemption_disable();
extern queue sleep_queue;
extern queue ready_queue;

void print_queue_info(const char *name, queue *q) {
    int count = 0;
    thread_lt *curr = q->head;
    printf("%s: ", name);
    while(curr) {
        printf("T%d ", curr->id);
        count++;
        curr = curr->next;
    }
    printf("(count=%d)\n", count);
}

void sleeper(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d (tid=%d): Going to sleep for 100ms...\n", id, current_thread->id);
    thread_sleep(100);
    printf("Thread %d (tid=%d): Woke up!\n", id, current_thread->id);
}

void sleeper_staggered(void *arg) {
    int delay = *(int*)arg;
    printf("Thread %d: Sleeping for %dms...\n", current_thread->id, delay);
    thread_sleep(delay);
    printf("Thread %d: Woke up after %dms!\n", current_thread->id, delay);
}

int main() {
    printf("=== Sleep Queue Diagnostic Test ===\n\n");
    
    init();
    preemption_init(10);
    
    printf("--- Test 1: Simple Sleep Test ---\n");
    int ids[] = {1, 2, 3};
    thread_lt *t1 = thread_create(sleeper, &ids[0]);
    thread_lt *t2 = thread_create(sleeper, &ids[1]);
    thread_lt *t3 = thread_create(sleeper, &ids[2]);
    
    // Give them a moment to start sleeping
    thread_sleep(20);
    
    printf("\nQueue status while threads sleeping:\n");
    print_queue_info("Ready queue", &ready_queue);
    print_queue_info("Sleep queue", &sleep_queue);
    
    printf("\nWaiting for threads to wake...\n");
    thread_join(t1);
    thread_join(t2);
    thread_join(t3);
    
    printf("\nQueue status after join:\n");
    print_queue_info("Ready queue", &ready_queue);
    print_queue_info("Sleep queue", &sleep_queue);
    
    printf("\n--- Test 2: Staggered Sleep Times ---\n");
    int delays[] = {50, 100, 150};
    thread_lt *s1 = thread_create(sleeper_staggered, &delays[0]);
    thread_lt *s2 = thread_create(sleeper_staggered, &delays[1]);
    thread_lt *s3 = thread_create(sleeper_staggered, &delays[2]);
    
    thread_join(s1);
    thread_join(s2);
    thread_join(s3);
    
    printf("\n--- Test 3: Many Sleepers ---\n");
    thread_lt *sleepers[10];
    int sleep_ids[10];
    for(int i = 0; i < 10; i++) {
        sleep_ids[i] = i;
        sleepers[i] = thread_create(sleeper, &sleep_ids[i]);
    }
    
    thread_sleep(20);
    printf("Sleep queue with 10 threads:\n");
    print_queue_info("Sleep queue", &sleep_queue);
    
    for(int i = 0; i < 10; i++) {
        thread_join(sleepers[i]);
    }
    
    printf("\n=== Diagnostic Complete ===\n");
    preemption_disable();
    return 0;
}