# Syscall ABI

This document describes the user-to-kernel syscall ABI for the RV64 operating system.

The goal is to keep the ABI very close to Linux on RISC-V so that the model is familiar and easy to extend later.

## Overview

When an `ecall` is executed in U-mode:

1. The processor traps into S-mode
2. The supervisor trap handler detects an environment call from U-mode
3. The syscall number is read from register `a7`
4. Arguments are read from registers `a0` through `a5`
5. The syscall handler places the return value in `a0`
6. The trapped instruction is skipped by advancing `sepc`/`epc` by 4
7. Control returns to user mode with `sret`

## Register Convention

This ABI follows the Linux RISC-V syscall calling convention.

- `a7` = syscall number
- `a0` = argument 0
- `a1` = argument 1
- `a2` = argument 2
- `a3` = argument 3
- `a4` = argument 4
- `a5` = argument 5

Return values:

- `a0` = primary return value
- negative values in `a0` indicate an error

## Current Syscalls

### `write`
- Number: `64`
- Linux-compatible: yes

Arguments:
- `a0` = file descriptor
- `a1` = buffer pointer
- `a2` = buffer length

Return:
- number of bytes written on success
- negative error code on failure

Current behavior:
- supports file descriptors:
  - `1` = stdout
  - `2` = stderr
- output is written to the UART
- other file descriptors return an error

### `exit`
- Number: `93`
- Linux-compatible: yes

Arguments:
- `a0` = exit status

Return:
- does not return

Current behavior:
- prints a message to the UART
- halts the current task by panicking for now

## Error Model

This ABI follows the Linux style of returning negative error codes in `a0`.

Examples:
- `-1` = generic failure
- `-9` = bad file descriptor
- `-14` = bad address
- `-38` = syscall not implemented

At the moment, only a small subset is used.

## Trap Routing

The supervisor trap handler must route:

- exception code `8`, environment call from U-mode, syscall handler

The syscall handler is responsible for advancing `frame->epc` by 4 after handling the call.

## Accessing User Memory

For syscalls that receive user pointers, the kernel must copy data from user memory safely.

The kernel must not blindly trust user pointers.

In S-mode, reading pages marked with the `U` bit may require setting `sstatus.SUM` while copying user memory.

## Notes

This is a minimal ABI intended for early kernel bring-up.

Future work may include:

- `read`
- `openat`
- `close`
- `brk` or `mmap`
- `exit_group`
- process scheduling
- signal support
- ELF program loading