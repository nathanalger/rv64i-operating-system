ENTRY(_start)

SECTIONS
{
    . = 0x80200000;

    .text : ALIGN(4096)
    {
        _kernel_start = .;
        _text_start = .;

        *(.text.init)
        *(.text*)

        . = ALIGN(4096);
        _text_end = .;
    }

    .rodata : ALIGN(4096)
    {
        _rodata_start = .;

        *(.rodata*)

        . = ALIGN(4096);
        _rodata_end = .;
    }

    .data : ALIGN(4096)
    {
        _data_start = .;

        *(.data*)

        . = ALIGN(4096);
        _data_end = .;
    }

    .bss : ALIGN(4096)
    {
        __bss_start = .;

        *(.bss)
        *(.sbss)
        *(.sbss*)
        *(COMMON)

        . = ALIGN(16);
        _kernel_stack_bottom = .;
        *(.bss.stack)
        . = ALIGN(16);
        _kernel_stack_top = .;

        . = ALIGN(16);
        _supervisor_trap_stack_bottom = .;
        *(.bss.trapstack)
        . = ALIGN(16);
        _supervisor_trap_stack_top = .;

        . = ALIGN(16);
        _machine_trap_stack_bottom = .;
        *(.bss.mtrapstack)
        . = ALIGN(16);
        _machine_trap_stack_top = .;

        *(.bss.*)

        . = ALIGN(4096);
        __bss_end = .;
    }

    . = ALIGN(4096);
    _kernel_end = .;
}