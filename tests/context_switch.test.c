#include "../src/context/context.h"
#include <stdio.h>
#include <stdlib.h>
// #include <cstddef>
#define STACK_SIZE (64 * 1024)

void *alloc_stack(void){
    void *stack = malloc(STACK_SIZE);
    return (char *)stack + STACK_SIZE; // <- top of the stack
}

context_t ctx_main, ctx_thread;

void thread_fn(void *arg){
    printf("In thread\n");
    context_switch(&ctx_thread, &ctx_main);
}

int main(){
    void *stack = alloc_stack();
    context_init(&ctx_thread, thread_fn, 0, stack);

    printf("Switching to thread\n");
    context_switch(&ctx_main, &ctx_thread);
    printf("back in main\n");
}