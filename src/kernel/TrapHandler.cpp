#include "TrapHandler.hpp"
#include "Panic.hpp"
#include "UART.hpp"

void trap_handler(uint64_t code, TrapFrame *frame)
{
   switch (code)
   {
   case 2:
      panic("Illegal instruction observed");
      return;
   case 3: // Breakpoint
      frame->epc += 4;
      return;

   case 5 ... 7:
      panic("access fault");
      return;

   case 8: // ECALL
   case 9:
      uart_puts("ecall handle: unimplemented\n");
      frame->epc += 4;
      return;

   case 12: // Instruction Page Fault
      panic("Observed instruction page fault: unimplemented");
      return;
   case 13: // Load Page Fault
      panic("Observed load page fault: unimplemented");
      return;
   case 15: // Store Page Fault
      panic("Observed store page fault: unimplemented");
      return;

   default:
      panic("Unknown fatal trap observed.");
      return;
   }
}