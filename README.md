# rv64i-operating-system
This is a simple operating system meant to run on RISC-V 64-bit based processors. Currently, it is capable of parsing a device tree and printing to the UART.
This is designed to be compatible with another project of mine, the [RV64I System Simulator](https://github.com/nathanalger/rv64i-simulator).

## Background

This project is being developed as a personal learning experience in operating system design by a Computer Engineering student at Michigan Technological University.

## Capabilities

This project is actively in development, so many features may be broken or unimplemented. Here is a general list of functional aspects of this project.

- Device Tree Parsing in kernel stage
- UART compatibility
- Standard Library Independent

## Future Goals

- Memory Management (malloc and similar syscalls)
- Expanded kernel subsystem
- SBI S- and U- Mode Management
- Additional Device Support

## Docs

I will try my best to keep up with documentation of important aspects of this project. Here is a basic documentation tree:

### Libraries

Below is a list of some documentation of the libraries utilized to help make this project work properly.

- [Libfdt](docs/lib/libfdt.md)

### Source Code

#### ARCH/RISC-V

- [UART](docs/arch/riscv/UART.md)

#### Kernel

- [Main](docs/src/kernel/main.md)

#### Utilities

- [DTB](docs/src/utility/DTB.md)
- [Memory](docs/src/utility/memory.md)
- [PrintHex](docs/src/utility/printHex.md)