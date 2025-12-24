.global context_switch
.type context_switch, @function

context_switch:
    # rdi = old
    # rsi = new

    # Save callee-saved registers
    mov %rsp, 0(%rdi)
    mov %rbx, 8(%rdi)
    mov %rbp, 16(%rdi)
    mov %r12, 24(%rdi)
    mov %r13, 32(%rdi)
    mov %r14, 40(%rdi)
    mov %r15, 48(%rdi)

    # Save resume RIP
    leaq 1f(%rip), %rax
    mov %rax, 56(%rdi)

    # Restore new context
    mov 0(%rsi), %rsp
    mov 8(%rsi), %rbx
    mov 16(%rsi), %rbp
    mov 24(%rsi), %r12
    mov 32(%rsi), %r13
    mov 40(%rsi), %r14
    mov 48(%rsi), %r15
    mov 56(%rsi), %rax

    jmp *%rax

1:
    ret
