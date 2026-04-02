ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

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

        *(.bss*)
        *(.bss.stack)
        *(COMMON)

        . = ALIGN(4096);
        __bss_end = .;
    }

    _kernel_end = .;
}