.section .text
.align 2
.global _start

_start:
    li a0, 42
    li a7, 0 
    ecall

1:
    j 1b