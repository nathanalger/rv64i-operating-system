# RISC-V Kernel Boot Process

When building, there are two different binaries created. One is `kernel-m`, and the other is `kernel-s`. These are meant for two separate operating environments.

## Kernel-M

Kernel-M is built to run independently of a BIOS. Specifically, when running QEMU with the `--bios none` flag. The kernel will launch into M mode initially, and it will go through the [Bootstrapper](../../../../src/kernel/Machine.cpp) before initializing traps and paging in Supervisor mode. 

Specific files for this include:

- [kernel-m.ld](../../../../linker/kernel-m.ld)
- [dispatch_m.ld](../../../../src/arch/riscv/boot/dispatch_m.cpp)
- [m_entry.S](../../../../src/arch/riscv/boot/m_entry.S)

## Kernel-S

Kernel-S is a build that skips the boostraping stage under the assumption that it has been handled by another BIOS, such as OpenSBI. It immediately boots into [Supervisor mode](../../../../src/kernel/Supervisor.cpp) and initializes traps and paging.

Specific files for this include:

- [kernel-m.ld](../../../../linker/kernel-s.ld)
- [dispatch_s.ld](../../../../src/arch/riscv/boot/dispatch_s.cpp)
- [s_entry.S](../../../../src/arch/riscv/boot/s_entry.S)