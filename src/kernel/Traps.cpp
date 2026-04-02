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

   if (trap_is_interrupt(frame->scause))
      uart_puts("Type: Interrupt\n");
   else
      uart_puts("Type: Exception\n");

   uart_puts("Code: ");
   Utility::print_hex(trap_code(frame->scause));
   uart_puts("\n");

   uart_puts("sepc: ");
   Utility::print_hex(frame->sepc);
   uart_puts("\n");

   uart_puts("stval: ");
   Utility::print_hex(frame->stval);
   uart_puts("\n");

   // Example recovery cases:

   // Breakpoint
   if (!trap_is_interrupt(frame->scause) && trap_code(frame->scause) == 3)
   {
      uart_puts("Handling breakpoint\n");
      frame->sepc += 4;
      return;
   }

   // Environment call
   if (!trap_is_interrupt(frame->scause) &&
       (trap_code(frame->scause) == 8 || trap_code(frame->scause) == 9))
   {
      uart_puts("Handling ecall\n");
      frame->sepc += 4;
      return;
   }

   // Fatal for now
   uart_puts("Fatal trap\n");
   while (1)
   {
   }
}

void traps_init()
{
   Utility::csr_write_stvec((uint64_t)supervisor_trap_entry);
}