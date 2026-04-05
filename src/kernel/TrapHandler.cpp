#include "TrapHandler.hpp"
#include "Panic.hpp"
#include "UART.hpp"
#include "CSR.hpp"

static void panic_unexpected_user_trap()
{
   panic("Trap reported as coming from user mode, but user mode is not implemented.");
}

void trap_handler(uint64_t code, TrapFrame *frame)
{
   uint64_t cause = frame->cause;
   const bool is_interrupt = (cause >> 63) != 0;

   if (is_interrupt)
   {
      handle_interrupt(code, frame);
      return;
   }

   handle_exception(code, frame);
}

void handle_interrupt(uint64_t code, TrapFrame *frame)
{
   (void)frame;

   switch (code)
   {
   case 1: // Supervisor software interrupt
      panic("Supervisor software interrupt observed: unimplemented");
      return;

   case 5: // Supervisor timer interrupt
      panic("Supervisor timer interrupt observed: unimplemented");
      return;

   case 9: // Supervisor external interrupt
      panic("Supervisor external interrupt observed: unimplemented");
      return;

   default:
      panic("Unknown interrupt observed.");
      return;
   }
}

void handle_exception(uint64_t code, TrapFrame *frame)
{
   uint64_t status = frame->status;
   const bool from_user = ((status & SSTATUS_SPP) == 0);

   if (from_user)
   {
      panic_unexpected_user_trap();
      return;
   }

   switch (code)
   {
   case 0: // Instruction address misaligned
      panic("Supervisor instruction address misaligned.");
      return;

   case 1: // Instruction access fault
      panic("Supervisor instruction access fault.");
      return;

   case 2: // Illegal instruction
      panic("Supervisor observed illegal instruction.");
      return;

   case 3: // Breakpoint
      advance_trap(frame);
      return;

   case 4: // Load address misaligned
      panic("Supervisor load address misaligned.");
      return;

   case 5: // Load access fault
      panic("Supervisor load access fault.");
      return;

   case 6: // Store/AMO address misaligned
      panic("Supervisor store/AMO address misaligned.");
      return;

   case 7: // Store/AMO access fault
      panic("Supervisor store/AMO access fault.");
      return;

   case 8: // ECALL from U-mode
      panic("ECALL from user mode observed, but user mode is not implemented.");
      return;

   case 9: // ECALL from S-mode
      panic("Unexpected supervisor ecall.");
      return;

   case 12: // Instruction page fault
      panic("Supervisor instruction page fault.");
      return;

   case 13: // Load page fault
      panic("Supervisor load page fault.");
      return;

   case 15: // Store/AMO page fault
      panic("Supervisor store/AMO page fault.");
      return;

   default:
      panic("Unknown fatal exception observed.");
      return;
   }
}

void advance_trap(TrapFrame *frame)
{
   frame->epc += 4;
}