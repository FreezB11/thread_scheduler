/// @file:stack.c
#include "stack.h"
 
void *alloc_stack(void){
    void *stack = malloc(STACK_SIZE);
    return (char *)stack + STACK_SIZE; // <- top of the stack
}