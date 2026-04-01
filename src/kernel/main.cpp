#include <stdint.h>
#include "UART.hpp"
#include "printHex.hpp"
#include "DTB.hpp"
#include "panic.hpp"

extern "C" char __bss_start[];
extern "C" char __bss_end[];
extern "C" char _kernel_end[];

extern "C" void kernel_main(const void* dtb)
{
    if (!dtb_init(dtb))
    {
        uart_puts("DTB init failed\n");
        while (1) { asm volatile("wfi"); }
    }

    uint64_t ram_base = 0;
    uint64_t ram_size = 0;
    uint64_t new_uart_base = 0;

    if(!dtb_get_uart_base(new_uart_base)) panic("Failed to fetch UART base address!");

    if (!dtb_get_memory_range(ram_base, ram_size))
    {
        panic("Failed to read memory from DTB!");
    }

    panic("Reached end of execution!");
}