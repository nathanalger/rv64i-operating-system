#include "Syscall.hpp"
#include "Traps.hpp"
#include "UART.hpp"

void syscall_handle(TrapFrame *frame)
{
   const uint64_t syscall_number = frame->a7;

   switch (syscall_number)
   {
   case 0:
      uart_puts("syscall 0\n");
      frame->a0 = 0;
      break;

   default:
      uart_puts("unknown syscall\n");
      frame->a0 = (uint64_t)-1;
      break;
   }

   frame->epc += 4;
}