.global thread_trampoline
thread_trampoline:
    mov %r12, %rdi  # fn goes to first arg
    mov %r13, %rsi  # arg goes to second arg
    call *%r12      # call fn(arg)
    call thread_exit # should nvr return, but to be safe