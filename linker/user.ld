OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x00400000;

    __user_text_start = .;
    .text : ALIGN(16)
    {
        *(.text*)
    }
    __user_text_end = .;

    __user_rodata_start = .;
    .rodata : ALIGN(16)
    {
        *(.rodata*)
    }
    __user_rodata_end = .;

    __user_data_start = .;
    .data : ALIGN(16)
    {
        *(.data*)
    }
    __user_data_end = .;

    __user_bss_start = .;
    .bss : ALIGN(16)
    {
        *(.bss*)
        *(COMMON)
    }
    __user_bss_end = .;

    __user_image_end = .;
}