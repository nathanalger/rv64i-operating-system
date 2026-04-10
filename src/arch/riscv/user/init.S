.section .text
.align 2
.global _start

_start:
    # write(1, prompt, prompt_end - prompt)
    li  a0, 1
    la  a1, prompt
    la  a2, prompt_end
    sub a2, a2, a1
    li  a7, 64
    ecall

    # read(0, input_buffer, 64)
    li  a0, 0
    la  a1, input_buffer
    li  a2, 64
    li  a7, 63
    ecall

    # Save bytes_read
    mv  s0, a0

    # write(1, echo_prefix, echo_prefix_end - echo_prefix)
    li  a0, 1
    la  a1, echo_prefix
    la  a2, echo_prefix_end
    sub a2, a2, a1
    li  a7, 64
    ecall

    # write(1, input_buffer, bytes_read)
    li  a0, 1
    la  a1, input_buffer
    mv  a2, s0
    li  a7, 64
    ecall



    # exit_group(0)
    li  a0, 0
    li  a7, 94
    ecall

1:
    j 1b

.section .rodata
.align 2
prompt:
    .ascii "Type something and press enter: "
prompt_end:

echo_prefix:
    .ascii "\nYou typed: "
echo_prefix_end:

.section .bss
.align 3
input_buffer:
    .skip 64