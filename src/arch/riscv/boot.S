.section .text.init
.globl _start

_start:
    la sp, stack_top

    # Save DTB pointer from a1 before calling C++
    mv s0, a1

    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bge t0, t1, bss_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

bss_done:
    mv a0, s0
    call dispatch

1:
    j 1b

.section .bss.stack
.align 16
stack_bottom:
    .space 8192
stack_top: