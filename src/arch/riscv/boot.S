.section .text.init
.globl _start

_start:
    la sp, stack_top        # set stack pointer to top of memory (8KiB in this case)
    la t0, __bss_start      # set t0 to the start of bss in linker
    la t1, __bss_end        # set t1 to the end of bss in linker

clear_bss:                  # clears bss
    bge t0, t1, bss_done    
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

bss_done:                   # once bss is clear, boot
    call kernel_main

1:                          # after execution, infinite loop
    j 1b

.section .bss.stack
.align 16

stack_bottom:
    .space 8192

stack_top: