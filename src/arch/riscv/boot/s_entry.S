.section .text.init
.align 2
.globl _start

_start:
    la sp, stack_top

    # OpenSBI convention:
    # a0 = hartid
    # a1 = dtb pointer
    mv s0, a0
    mv s1, a1

    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bgeu t0, t1, bss_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

bss_done:
    mv a0, s0          # hartid
    mv a1, s1          # dtb
    call dispatch

1:
    j 1b

.section .bss.stack
.align 16
stack_bottom:
    .space 8192
stack_top: