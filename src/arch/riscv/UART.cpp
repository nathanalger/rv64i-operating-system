#include "arch/riscv/UART.hpp"

static uint64_t g_uart_base = 0x10000000;

void uart_init(uint64_t base_address)
{
    g_uart_base = base_address;
}

uint64_t uart_base()
{
    return g_uart_base;
}

static volatile unsigned char* uart_reg()
{
    return (volatile unsigned char*)g_uart_base;
}

void uart_putc(char c)
{
    *uart_reg() = (unsigned char)c;
}

void uart_puts(const char* s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}