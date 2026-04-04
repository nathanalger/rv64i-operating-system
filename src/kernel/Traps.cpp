#include "Traps.hpp"
#include "CSR.hpp"
#include "UART.hpp"
#include "PrintHex.hpp"

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
   uart_puts("Trap Encountered\n");

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

   // Example recovery cases:

   // Breakpoint
   if (!trap_is_interrupt(frame->cause) && trap_code(frame->cause) == 3)
   {
      uart_puts("Handling breakpoint\n");
      frame->epc += 4;
      return;
   }

   // Environment call
   if (!trap_is_interrupt(frame->cause) &&
       (trap_code(frame->cause) == 8 || trap_code(frame->cause) == 9))
   {
      uart_puts("Handling ecall\n");
      frame->epc += 4;
      return;
   }

   // Fatal for now
   uart_puts("Fatal trap\n");
   while (1)
   {
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

   while (1)
   {
   }
}

void machine_traps_init()
{
   csr_write_mtvec((uint64_t)machine_trap_entry);
}

void supervisor_traps_init()
{
   csr_write_stvec((uint64_t)supervisor_trap_entry);
}