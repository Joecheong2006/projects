.global _start

hello:
    .ascii "hello, world\n"

_exit:
    movq $60, %rax
    movq $0, %rdi
    syscall

_start:
    movq %rsp, %rbp

    movq $1, %rax
    movq $1, %rdi
    leaq hello, %rsi
    movq $13, %rdx
    syscall

    call _exit
    pop %rbp
