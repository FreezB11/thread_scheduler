#include <context.h>
#include <stdint.h>
#include <stdlib.h>
#include <thread_lt.h>

/* Save current CPU context into ctx */
void context_capture(context_t *ctx) {
    __asm__ volatile (
        "mov %%rsp, 0(%0)\n"
        "mov %%rbx, 8(%0)\n"
        "mov %%rbp, 16(%0)\n"
        "mov %%r12, 24(%0)\n"
        "mov %%r13, 32(%0)\n"
        "mov %%r14, 40(%0)\n"
        "mov %%r15, 48(%0)\n"
        "leaq 1f(%%rip), %%rax\n"
        "mov %%rax, 56(%0)\n"
        "1:\n"
        :
        : "r"(ctx)
        : "rax", "memory"
    );
}

/* Entry point for new threads */
// __attribute__((noreturn))
static void thread_trampoline(void) {
    void (*fn)(void *);
    void *arg;

    __asm__ volatile (
        "mov %%r12, %0\n"
        "mov %%r13, %1\n"
        : "=r"(fn), "=r"(arg)
    );

    fn(arg);
    thread_exit();

    // __builtin_unreachable();
}

/* Initialize a fresh context */
// void context_init(context_t *ctx, void (*fn)(void *), void *arg, void *stack_top) {
//     uint64_t *sp = (uint64_t *)((uintptr_t)stack_top & ~0xF);

//     *(--sp) = 0;  // fake return address

//     ctx->rsp = (uint64_t)sp;
//     ctx->rip = (uint64_t)thread_trampoline;

//     ctx->rbx = 0;
//     ctx->rbp = 0;
//     ctx->r12 = (uint64_t)fn;
//     ctx->r13 = (uint64_t)arg;
//     ctx->r14 = 0;
//     ctx->r15 = 0;
// }
void context_init(context_t *ctx, void (*fn)(void *), void *arg, void *stack_top) {
    // 1. Align stack to 16 bytes
    uint64_t *sp = (uint64_t *)((uintptr_t)stack_top & ~0xF);
    
    // 2. Setup the struct
    ctx->rsp = (uint64_t)sp;
    ctx->rip = (uint64_t)thread_trampoline; // When we switch to this, JMP here
    
    ctx->r12 = (uint64_t)fn;  // Trampoline expects fn in R12
    ctx->r13 = (uint64_t)arg; // Trampoline expects arg in R13
    
    // Initialize other regs to 0 to avoid garbage
    ctx->rbx = 0;
    ctx->rbp = 0;
    ctx->r14 = 0;
    ctx->r15 = 0;
}
