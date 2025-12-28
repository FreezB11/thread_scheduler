///@file: ex_matrix.c
#include <thread_lt.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000
#define NUM_THREADS 4

int A[SIZE], B[SIZE], C[SIZE];

typedef struct {
    int start;
    int end;
    int id;
} range_t;

void vector_add(void *arg) {
    range_t *r = (range_t*)arg;
    printf("Worker %d processing indices %d to %d\n", r->id, r->start, r->end);
    
    for (int i = r->start; i < r->end; i++) {
        C[i] = A[i] + B[i];
    }
    free(r); // Clean up arg
}

int main() {
    init();

    // Initialize data
    for(int i=0; i<SIZE; i++) { A[i] = i; B[i] = i*2; }

    thread_lt *workers[NUM_THREADS];
    int chunk = SIZE / NUM_THREADS;

    // Launch threads
    for(int i=0; i<NUM_THREADS; i++) {
        range_t *r = malloc(sizeof(range_t));
        r->id = i;
        r->start = i * chunk;
        r->end = (i == NUM_THREADS - 1) ? SIZE : (i + 1) * chunk;
        
        workers[i] = thread_create(vector_add, r);
    }

    // Join
    for(int i=0; i<NUM_THREADS; i++) thread_join(workers[i]);

    // Verify
    printf("A[0]=%d, A[999]=%d,, B[0]=%d, B[999]=%d\n", A[0], A[999],B[0], B[999]);
    printf("Verification: C[0]=%d, C[999]=%d\n", C[0], C[999]);
    return 0;
}
