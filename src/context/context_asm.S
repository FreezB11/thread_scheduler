.global context_switch
.type context_switch, @function

context_switch:
    # rdi = old context
    # rsi = new context

    # --- save old context ---
    mov %rbx, 8(%rdi)
    mov %rbp, 16(%rdi)
    mov %r12, 24(%rdi)
    mov %r13, 32(%rdi)
    mov %r14, 40(%rdi)
    mov %r15, 48(%rdi)

    # save stack ptr
    mov %rsp, 0(%rdi)

    # save rip (next intruction)
    mov (%rsp), %rax
    mov %rax, 56(%rdi)

    # --- load new context ---
    mov 0(%rsi), %rsp
    mov 8(%rsi), %rbx
    mov 16(%rsi), %rbp
    mov 24(%rsi), %r12
    mov 32(%rsi), %r13
    mov 40(%rsi), %r14
    mov 48(%rsi), %r15
    mov 56(%rsi), %rax

    jmp *%rax