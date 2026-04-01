# libfdt Usage

This project uses libfdt, which is an open source utility for building and parsing device trees. It is used to parse memory size, device locations, etc. after the kernel boot phase. 

libfdt is double licensed under the GPL and BSD License.

## uart_putc

Prints a character to the UART.

```cpp
void uart_putc(char c);
```

## uart_puts

Prints a character array string to the UART.

```cpp
void uart_puts(const char* c);
```