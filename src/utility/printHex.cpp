#include "UART.hpp"
#include <cstdint>

void print_hex(uint64_t value)
{
    const char* hex = "0123456789ABCDEF";

    uart_puts("0x");

    for (int i = 60; i >= 0; i -= 4)
    {
        uart_putc(hex[(value >> i) & 0xF]);
    }
}