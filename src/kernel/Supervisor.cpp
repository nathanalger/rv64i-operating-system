#include "CSR.hpp"
#include "UART.hpp"
#include "Traps.hpp"
#include "PagingTests.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"

extern "C" char _rodata_start[];
extern "C" char _supervisor_trap_stack_top[];

extern PageTable *g_kernel_root;

extern "C" void machine_main(const void *dtb);

extern "C" void supervisor_main(const void *dtb)
{
   (void)dtb;

   uart_puts("Supervisor Mode Reached\n");
   supervisor_traps_init();

   panic("Unexpectedly reached end of supervisor mode execution.");
}