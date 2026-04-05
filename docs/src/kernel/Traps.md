# Kernel Trap Policy

## 1. Expected Traps

## 2. User Faults

This trap will be the result of a user program having a bug, attempting illegal access, etc. This will kill the violating task, but will generally not result in a panic.

sstatus.SPP = 0

## 3. Kernel Faults

Kernel faults are a type of fault that is essentially always a result of a bug. These will result in a panic.

sstatus.SPP = 1