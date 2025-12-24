#include "context.h"
#include <stdlib.h>

extern context_t ctx_main;
extern context_t ctx_thread;

__attribute__((noreturn))
void uthr_exit(void) {
    context_switch(&ctx_thread, &ctx_main);
    __builtin_unreachable();
}

// __attribute__((noreturn))
// void uthr_exit(void) {
//     // while (1) {
//     //     __asm__ volatile("hlt");
//     // }
//     exit(0);
// }

__attribute__((noreturn))
void thread_trampoline(void) {
    void (*fn)(void *);
    void *arg;

    __asm__ volatile (
        "mov %%r12, %0\n"
        "mov %%r13, %1\n"
        : "=r"(fn), "=r"(arg)
    );

    fn(arg);
    uthr_exit();
}


void context_init(context_t *ctx, void ( *fn)(void *), void *arg, void *stack_top){
    uint64_t *sp = (uint64_t *)stack_top;

    //align
    sp = (uint64_t *)((uintptr_t)sp & ~0xF);

    // dummy
    *(--sp) = 0;
    ctx->rsp = (uint64_t)sp;
    ctx->rip = (uint64_t)thread_trampoline;

    ctx->rbx = ctx->rbp = 0;

    ctx->r12 = (uint64_t)fn;
    ctx->r13 = (uint64_t)arg;
    ctx->r14 = ctx->r15 = 0;
}
