// ///@file: benchmark_cv.c
// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <scheduler.h>
// #include <mutex.h>
// #include <cond.h>

// // --- Config ---
// #define NUM_PINGS 100 // Start small (100k)

// // --- Globals ---
// mutex_t mtx;
// convar cv_ping; // Signal for T1
// convar cv_pong; // Signal for T2
// int pings_sent = 0;

// // --- Timer ---
// double get_time_sec() {
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return ts.tv_sec + ts.tv_nsec * 1e-9;
// }

// void worker1(void *arg) {
//     (void)arg;
//     for (int i = 0; i < NUM_PINGS; i++) {
//         mutex_lock(&mtx);
//         // Signal T2 and wait for response
//         cond_signal(&cv_pong);
//         cond_wait(&cv_ping, &mtx);
//         pings_sent++;
//         mutex_unlock(&mtx);
//     }
//     // Wake up T2 one last time so it can exit
//     mutex_lock(&mtx);
//     cond_signal(&cv_pong);
//     mutex_unlock(&mtx);
// }

// void worker2(void *arg) {
//     (void)arg;
//     for (int i = 0; i < NUM_PINGS; i++) {
//         mutex_lock(&mtx);
//         // Wait for signal from T1
//         cond_wait(&cv_pong, &mtx);
//         // Signal T1 back
//         cond_signal(&cv_ping);
//         mutex_unlock(&mtx);
//     }
// }

// void worker3(void *arg){
//     for(int i = 0; i < NUM_PINGS; i++){
//         printf("hello\n");
//     }
// }

// int main() {
//     init();
//     mutex_init(&mtx);
//     cond_init(&cv_ping);
//     cond_init(&cv_pong);

//     printf("Starting CV Benchmark (Ping-Pong)...\n");
//     double start = get_time_sec();

//     int *p1 = malloc(sizeof(int)); *p1 = 1;
//     int *p2 = malloc(sizeof(int)); *p2 = 2;
//     thread_lt *t1 = thread_create(worker1, p1);
//     thread_lt *t2 = thread_create(worker2, p2);
//     // thread_lt *t3 = thread_create(worker2, NULL);

//     // Bootstrap: T2 waits first, so T1 needs to run first.
//     // If T2 runs first, it hits wait(). Then T1 runs. Perfect.
    
//     thread_join(t1);
//     // thread_join(t2);
//     // thread_join(t3);

//     double end = get_time_sec();
//     double duration = end - start;

//     printf("Done %d round-trips in %.4fs\n", NUM_PINGS, duration);
//     printf("Throughput: %.0f switches/sec\n", (NUM_PINGS * 2.0) / duration);
//     return 0;
// }
///@file: benchmark_pc.c
#define _POSIX_C_SOURCE 200809L
#include <thread_lt.h>
#include <mutex.h>
#include <cond.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Configuration ---
#define NUM_ITEMS 1000000 // 1 Million Items
#define BUFFER_SIZE 10    // slightly larger buffer to allow burstiness

// --- Globals ---
int buffer[BUFFER_SIZE];
int count = 0;
int in = 0;
int out = 0;

long long producer_sum = 0; // Checksum
long long consumer_sum = 0; // Checksum

mutex_t mutex;
convar not_full;
convar not_empty;

// --- Timer Helper ---
double get_time_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void producer(void *arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        mutex_lock(&mutex);
        
        while (count == BUFFER_SIZE) {
            cond_wait(&not_full, &mutex);
        }
        
        buffer[in] = i; // Store simple integer
        in = (in + 1) % BUFFER_SIZE;
        count++;
        producer_sum += i; // Track what we produced
        
        cond_signal(&not_empty);
        mutex_unlock(&mutex);
    }
}

void consumer(void *arg) {
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        mutex_lock(&mutex);
        
        while (count == 0) {
            cond_wait(&not_empty, &mutex);
        }
        
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        consumer_sum += item; // Track what we consumed
        
        cond_signal(&not_full);
        mutex_unlock(&mutex);
    }
}

int main() {
    init();
    mutex_init(&mutex);
    cond_init(&not_full);
    cond_init(&not_empty);

    printf("==========================================\n");
    printf("   PRODUCER-CONSUMER BENCHMARK (1M Items) \n");
    printf("==========================================\n");
    printf("Buffer Size: %d\n", BUFFER_SIZE);

    thread_lt *t1 = thread_create(producer, NULL);
    thread_lt *t2 = thread_create(consumer, NULL);

    printf("Running... ");
    fflush(stdout);

    double start = get_time_sec();
    
    thread_join(t1);
    thread_join(t2);
    
    double end = get_time_sec();
    double duration = end - start;

    printf("Done!\n");
    printf("------------------------------------------\n");
    printf("Time:           %.4f seconds\n", duration);
    printf("Throughput:     %.0f items/sec\n", NUM_ITEMS / duration);
    
    // Each item involves at least 2 lock/unlocks.
    // If buffer fills/empties often, it causes context switches.
    // With Size=10, we switch less often than Ping-Pong (Size=1).
    
    printf("------------------------------------------\n");
    if (producer_sum == consumer_sum) {
        printf("CHECKSUM PASS: %lld\n", producer_sum);
    } else {
        printf("CHECKSUM FAIL! Prod: %lld, Cons: %lld\n", producer_sum, consumer_sum);
    }
    printf("==========================================\n");

    return 0;
}
