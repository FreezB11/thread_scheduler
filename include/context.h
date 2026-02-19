///@file: context.h
#pragma once
#include <stdint.h>

typedef struct context {
    uint64_t rsp; //stack ptr
    uint64_t rbx; // calle saved reg
    uint64_t rbp;// frame ptr
    uint64_t r12;//callee daved (we use fn)
    uint64_t r13;//calle saved (we use arg here)
    uint64_t r14;//callee saved
    uint64_t r15;//callee save
    uint64_t rip;// instructn ptr
} context_t;

void context_init(context_t *ctx, void (*fn)(void *), void *arg, void *stack_top);
void context_switch(context_t *old, context_t *new_);
void context_capture(context_t *ctx);