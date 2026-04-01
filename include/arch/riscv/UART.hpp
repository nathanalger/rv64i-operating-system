#include <stdint.h>

/**
 * Initialize the UART at a specific address
 */
void uart_init(uint64_t base_address);

/**
 * Fetch the UART base as an unsigned integer
 */
uint64_t uart_base();

/**
 * Print a character to the UART
 */
void uart_putc(char c);

/**
 * Print a string to the UART interface
 */
void uart_puts(const char* s);