# Syscall ABI

This document defines the system call ABI for user programs running on this RV64 operating system.

The ABI is intentionally designed to be very close to Linux on RISC-V so that it is familiar, easy to extend, and compatible in spirit with standard RISC-V syscall conventions.

## AI Disclosure

This file is absolutely written by AI and I will not try to hide it. It is, however, checked for accuracy.

## Overview

A user program invokes a system call with the `ecall` instruction.

When an `ecall` is executed in U-mode:

1. The processor traps into S-mode
2. The trap handler identifies the exception as an environment call from U-mode
3. The syscall number is read from register `a7`
4. Arguments are read from registers `a0` through `a5`
5. The syscall handler performs the requested operation
6. The return value is placed in `a0`
7. The trapped instruction is skipped by advancing `sepc` by 4
8. Execution returns to U-mode with `sret`

## Register Convention

This ABI follows the Linux RISC-V syscall convention.

### Syscall Number
- `a7` = syscall number

### Arguments
- `a0` = argument 0
- `a1` = argument 1
- `a2` = argument 2
- `a3` = argument 3
- `a4` = argument 4
- `a5` = argument 5

### Return Value
- `a0` = return value

On success:
- `a0 >= 0`

On failure:
- `a0 < 0`
- negative values are Linux-style error codes

## Current Syscalls

### `read`
- syscall number: `63`
- Linux-compatible number: yes

#### Arguments
- `a0` = file descriptor
- `a1` = pointer to user buffer
- `a2` = number of bytes to read

#### Return Value
- number of bytes read on success
- negative error code on failure

#### Current Supported File Descriptors
- `0` = stdin

#### Current Behavior
The kernel reads bytes from the UART and copies them into the user buffer.

Current input behavior is line-oriented for interactive use:
- input is read from UART one byte at a time
- carriage return is normalized to newline
- backspace and delete are handled during line editing
- input currently stops early when a newline is received

Unsupported file descriptors return:
- `-EBADF` (`-9`)

Bad user pointers return:
- `-EFAULT` (`-14`)

### `write`
- syscall number: `64`
- Linux-compatible number: yes

#### Arguments
- `a0` = file descriptor
- `a1` = pointer to user buffer
- `a2` = number of bytes to write

#### Return Value
- number of bytes written on success
- negative error code on failure

#### Current Supported File Descriptors
- `1` = stdout
- `2` = stderr

#### Current Behavior
The kernel copies bytes from the user buffer and writes them to the UART.

Unsupported file descriptors return:
- `-EBADF` (`-9`)

Bad user pointers return:
- `-EFAULT` (`-14`)

### `exit`
- syscall number: `93`
- Linux-compatible number: yes

#### Arguments
- `a0` = exit status

#### Return Value
This syscall does not return to the caller.

#### Current Behavior
The kernel prints the exit status and stops execution in an idle loop for now.

In the future, this should terminate the current task and return control to a scheduler.

### `exit_group`
- syscall number: `94`
- Linux-compatible number: yes

#### Arguments
- `a0` = exit status

#### Return Value
This syscall does not return to the caller.

#### Current Behavior
This currently behaves the same as `exit`.

That is acceptable for the current single-task system. In the future, `exit_group` should terminate all threads in the current process, matching normal Linux behavior more closely.

## Error Codes

The kernel uses Linux-style negative error returns in `a0`.

Currently used:

- `-9` = `EBADF`
- `-14` = `EFAULT`
- `-38` = `ENOSYS`

### `EBADF`
Returned when a syscall uses an unsupported file descriptor.

### `EFAULT`
Returned when a syscall is given an invalid user pointer.

### `ENOSYS`
Returned when a syscall number is not implemented.

## Trap Routing

The supervisor trap handler must route:

- exception code `8` = environment call from U-mode

to:

- `syscall_handle(TrapFrame *frame)`

The syscall handler is responsible for advancing `frame->epc` by 4 after handling the `ecall`.

## User Memory Access

Syscalls that accept user pointers must safely copy memory between user space and kernel space.

The kernel must not blindly trust user-provided addresses.

Because the kernel runs in S-mode and user memory is marked with the `U` bit, copying between kernel and user memory may require temporarily setting `sstatus.SUM` during the copy operation.

Current implementation includes:
- copy from user for `write`
- copy to user for `read`

## Process Entry vs Syscall ABI

The process entry ABI is separate from the syscall ABI.

### Process Entry ABI
This defines register contents when a user task begins execution at `_start`.

Current convention:
- `a0` = hart ID
- `a1` = DTB pointer

### Syscall ABI
This defines register usage when a user program executes `ecall`.

Current convention:
- `a7` = syscall number
- `a0` to `a5` = syscall arguments
- `a0` = syscall return value

These two conventions must be kept separate.

## Notes on Instruction Size

This kernel is built with support for the RISC-V compressed instruction extension (`C`).

That means many instructions may be 2 bytes long.

However, `ecall` is still treated as a 4-byte instruction, so advancing `epc` by 4 after a syscall is correct.

## Current Limitations

The current syscall layer is intentionally minimal.

Notable limitations include:
- no scheduler yet
- no process table yet
- no file system yet
- `read` and `write` currently use the UART directly
- `exit` and `exit_group` currently halt in an idle loop instead of tearing down a task cleanly

## Future Work

Likely future syscalls include:

- `openat`
- `close`
- `brk`
- `mmap`
- scheduler interaction
- process creation and teardown
- a proper task exit path
- a real file descriptor layer
- device-independent stdin/stdout/stderr handling