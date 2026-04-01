#include "arch/riscv/UART.hpp"
#include "kernel/panic.hpp"

[[noreturn]] void _panic(const char* functionName, const char* msg)
{
    uart_puts("Kernel PANIC in ");
    uart_puts(functionName);
    uart_puts(": ");
    uart_puts(msg);
    uart_puts("\n");

    while (1)
    {
        asm volatile("wfi");
    }
}