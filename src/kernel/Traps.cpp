#include "Traps.hpp"
#include "CSR.hpp"
#include "UART.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"
#include "TrapHandler.hpp"

extern "C" char _supervisor_trap_stack_top[];

static inline bool trap_is_interrupt(uint64_t scause)
{
   return (scause >> 63) != 0;
}
static inline uint64_t trap_code(uint64_t scause)
{
   return scause & ~(1ULL << 63);
}

extern "C" void supervisor_trap_handler(TrapFrame *frame)
{
   uart_puts("Supervisor Trap Encountered\n");

   if (trap_is_interrupt(frame->cause))
      uart_puts("Type: Interrupt\n");
   else
      uart_puts("Type: Exception\n");

   uart_puts("Code: ");
   Utility::print_hex(trap_code(frame->cause));
   uart_puts("\n");

   uart_puts("sepc: ");
   Utility::print_hex(frame->epc);
   uart_puts("\n");

   uart_puts("stval: ");
   Utility::print_hex(frame->tval);
   uart_puts("\n");

   if (!trap_is_interrupt(frame->cause))
   {
      trap_handler(frame);
   }
}

extern "C" void machine_trap_handler(TrapFrame *frame)
{
   uart_puts("Machine trap encountered\n");

   uart_puts("mcause: ");
   Utility::print_hex(frame->cause);
   uart_puts("\n");

   uart_puts("mepc: ");
   Utility::print_hex(frame->epc);
   uart_puts("\n");

   uart_puts("mtval: ");
   Utility::print_hex(frame->tval);
   uart_puts("\n");

   panic("Machine trap observed, execution halted.");
}
#include "CSR.hpp"
#include "Traps.hpp"

extern "C" char _machine_trap_stack_top[];
extern "C" char _supervisor_trap_stack_top[];

void machine_traps_init()
{
   csr_write_mtvec((uint64_t)machine_trap_entry);
   csr_write_mscratch((uint64_t)_machine_trap_stack_top);
}

void supervisor_traps_init()
{
   csr_write_stvec((uint64_t)supervisor_trap_entry);
   csr_write_sscratch((uint64_t)_supervisor_trap_stack_top);
}