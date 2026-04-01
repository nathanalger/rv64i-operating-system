ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text : ALIGN(4)
    {
        *(.text.init)
        *(.text*)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    }

    .data : ALIGN(4)
    {
        *(.data*)
    }

    .bss : ALIGN(16)
    {
        __bss_start = .;

        *(.bss*)
        *(.bss.stack)
        *(COMMON)

        __bss_end = .;
    }

    . = ALIGN(4096);
    _kernel_end = .;
}