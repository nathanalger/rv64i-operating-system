.section .text
.align 2
.global _start

_start:
    # write(1, message, message_end - message)
    li  a0, 1
    la  a1, message
    la  a2, message_end
    sub a2, a2, a1
    li  a7, 64
    ecall

    # exit(0)
    li  a0, 0
    li  a7, 93
    ecall

1:
    j 1b

.section .rodata
message:
    .ascii "Hello from user mode via write syscall.\n"
message_end: