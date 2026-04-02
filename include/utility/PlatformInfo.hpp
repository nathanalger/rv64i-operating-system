#pragma once
#include <stdint.h>

struct PlatformInfo
{
    uint64_t ram_base;
    uint64_t ram_size;
    uint64_t uart_base;

    uint64_t dtb_addr;
    uint64_t dtb_size;
    uint64_t dtb_end;

    uint64_t kernel_start;
    uint64_t kernel_end;
    uint64_t bss_start;
    uint64_t bss_end;
};