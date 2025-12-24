#pragma once
#include <stdint.h>

typedef struct context{
    uint64_t rsp;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;    
}context_t;

//proto
void context_switch(context_t * old, context_t *new_);
void context_init(context_t *ctx, void ( *fn)(void *), void *arg, void *stack_top);