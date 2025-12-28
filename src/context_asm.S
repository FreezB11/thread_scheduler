/*
 * context.S - The Engine Room
 * 
 * We assume struct context_t layout:
 *   offset 0:  rsp
 *   offset 8:  rbx
 *   offset 16: rbp
 *   offset 24: r12
 *   offset 32: r13
 *   offset 40: r14
 *   offset 48: r15
 *   offset 56: rip  <-- CRITICAL
 */

.global context_switch
.type context_switch, @function

context_switch:
    /* RDI = old_ctx, RSI = new_ctx */

    /* 1. SAVE OLD STATE */
    movq %rsp, 0(%rdi)     /* Save RSP */
    movq %rbx, 8(%rdi)
    movq %rbp, 16(%rdi)
    movq %r12, 24(%rdi)
    movq %r13, 32(%rdi)
    movq %r14, 40(%rdi)
    movq %r15, 48(%rdi)

    /* Save RIP. 
       The 'call' instruction that invoked this function pushed the 
       Return Address (RIP) onto the stack. We read it from (%rsp).
    */
    movq (%rsp), %rax
    movq %rax, 56(%rdi)

    /* 2. LOAD NEW STATE */
    movq 0(%rsi), %rsp     /* Load new RSP */
    movq 8(%rsi), %rbx
    movq 16(%rsi), %rbp
    movq 24(%rsi), %r12
    movq 32(%rsi), %r13
    movq 40(%rsi), %r14
    movq 48(%rsi), %r15

    /* 3. JUMP TO NEW RIP */
    movq 56(%rsi), %rax    /* Load new RIP */
    jmp *%rax              /* Jump! */

/* 
 * void thread_trampoline(void)
 * This is the entry point for every new thread.
 * We expect:
 *   R12 = Function Pointer
 *   R13 = Argument
 */
.global thread_trampoline
.type thread_trampoline, @function
thread_trampoline:
    /* We are here because context_switch JMP'd to us.
       RSP is already set to the new thread's stack.
       R12 and R13 hold our data. */
    
    /* Call fn(arg) */
    /* System V ABI: 1st arg in RDI */
    movq %r13, %rdi  /* Move arg (R13) to RDI */
    
    /* Ensure stack is 16-byte aligned before call */
    /* We came from JMP, not CALL, so RSP is 16-byte aligned (from context_init) */
    
    call *%r12       /* Call fn() */
    
    /* If fn returns, we exit */
    call thread_exit
    
    /* Should never return */
    hlt
