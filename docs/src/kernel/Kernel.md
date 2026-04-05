# Kernel

There are two main stages in the kernel. The first being toe boostraping state, or Machine kernel. The second is the Supervisor kernel. The Machine kernel is used only with a lack of a BIOS to prepare the system. Read more about this [here](../arch/riscv/Boot.md).

# Machine Kernel

The [Machine Kernel](../../../src/kernel/Machine.cpp) is the entry point during boot for the M build of the kernel. It prepares the [Machine Trap Handler](../../../src/kernel/Traps.cpp), delegates the supervisor traps, and initializes PMP. 

After initialization, it then drops down into Supervisor mode and enters the [Supervisor Kernel](../../../src/kernel/Supervisor.cpp).

# Supervisor Kernel

The [Supervisor Kernel](../../../src/kernel/Supervisor.cpp) is the entry point for the S build of the kernel, skipping the boostrapping stage of the Machine Kernel. It initializes the Sv39 paging scheme, reads the device tree using [libfdt](../../lib/libfdt.md), and initializes the [Supervisor Trap Handler](../../../src/kernel/TrapHandler.cpp)