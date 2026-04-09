#include "TrapHandler.hpp"
#include "Panic.hpp"
#include "UART.hpp"
#include "PrintHex.hpp"
#include "CSR.hpp"
#include "Debug.hpp"
#include "Syscall.hpp"

[[noreturn]] void panic_trap(const char *message, TrapFrame *frame)
{
   Debug::prints(message);
   Debug::prints("\nsepc: ");
   Debug::print_hex(frame->epc);
   Debug::prints("\nscause: ");
   Debug::print_hex(frame->cause);
   Debug::prints("\nstval: ");
   Debug::print_hex(frame->tval);
   Debug::prints("\nsstatus: ");
   Debug::print_hex(frame->status);
   Debug::prints("\n");

   panic("Trap fatal.");
}

static uint64_t trap_code(uint64_t cause)
{
   return cause & 0x7FFFFFFFFFFFFFFFULL;
}

static bool trap_is_interrupt(uint64_t cause)
{
   return (cause >> 63) != 0;
}

static bool trap_from_user(const TrapFrame *frame)
{
   return (frame->status & SSTATUS_SPP) == 0;
}

void advance_trap(TrapFrame *frame)
{
   volatile uint16_t first_half = *(volatile uint16_t *)frame->epc;

   if ((first_half & 0x3) != 0x3)
      frame->epc += 2;
   else
      frame->epc += 4;
}

void handle_interrupt(TrapFrame *frame)
{
   const uint64_t code = trap_code(frame->cause);

   switch (code)
   {
   case 1: // Supervisor software interrupt
      panic_trap("Supervisor software interrupt observed: unimplemented.", frame);
      return;

   case 5: // Supervisor timer interrupt
      panic_trap("Supervisor timer interrupt observed: unimplemented.", frame);
      return;

   case 9: // Supervisor external interrupt
      panic_trap("Supervisor external interrupt observed: unimplemented.", frame);
      return;

   default:
      panic_trap("Unknown interrupt observed.", frame);
      return;
   }
}

void handle_supervisor_exception(TrapFrame *frame)
{
   const uint64_t code = trap_code(frame->cause);

   switch (code)
   {
   case 0: // Instruction address misaligned
      panic_trap("Supervisor instruction address misaligned.", frame);
      return;

   case 1: // Instruction access fault
      panic_trap("Supervisor instruction access fault.", frame);
      return;

   case 2: // Illegal instruction
      panic_trap("Supervisor illegal instruction.", frame);
      return;

   case 3: // Breakpoint
      advance_trap(frame);
      return;

   case 4: // Load address misaligned
      panic_trap("Supervisor load address misaligned.", frame);
      return;

   case 5: // Load access fault
      panic_trap("Supervisor load access fault.", frame);
      return;

   case 6: // Store/AMO address misaligned
      panic_trap("Supervisor store/AMO address misaligned.", frame);
      return;

   case 7: // Store/AMO access fault
      panic_trap("Supervisor store/AMO access fault.", frame);
      return;

   case 8: // ECALL from U-mode
      panic_trap("ECALL from U-mode reached supervisor exception path unexpectedly.", frame);
      return;

   case 9: // ECALL from S-mode
      panic_trap("Unexpected supervisor ecall.", frame);
      return;

   case 12: // Instruction page fault
      panic_trap("Supervisor instruction page fault.", frame);
      return;

   case 13: // Load page fault
      panic_trap("Supervisor load page fault.", frame);
      return;

   case 15: // Store/AMO page fault
      panic_trap("Supervisor store/AMO page fault.", frame);
      return;

   default:
      panic_trap("Unknown supervisor exception observed.", frame);
      return;
   }
}

void handle_user_exception(TrapFrame *frame)
{
   const uint64_t code = trap_code(frame->cause);

   switch (code)
   {
   case 0: // Instruction address misaligned
      panic_trap("User instruction address misaligned.", frame);
      return;

   case 1: // Instruction access fault
      panic_trap("User instruction access fault.", frame);
      return;

   case 2: // Illegal instruction
      panic_trap("User illegal instruction.", frame);
      return;

   case 3: // Breakpoint
      advance_trap(frame);
      return;

   case 4: // Load address misaligned
      panic_trap("User load address misaligned.", frame);
      return;

   case 5: // Load access fault
      panic_trap("User load access fault.", frame);
      return;

   case 6: // Store/AMO address misaligned
      panic_trap("User store/AMO address misaligned.", frame);
      return;

   case 7: // Store/AMO access fault
      panic_trap("User store/AMO access fault.", frame);
      return;

   case 8: // ECALL from U-mode
      syscall_handle(frame);
      return;

   case 12: // Instruction page fault
      panic_trap("User instruction page fault.", frame);
      return;

   case 13: // Load page fault
      panic_trap("User load page fault.", frame);
      return;

   case 15: // Store/AMO page fault
      panic_trap("User store/AMO page fault.", frame);
      return;

   default:
      panic_trap("Unknown user exception observed.", frame);
      return;
   }
}

void handle_exception(TrapFrame *frame)
{
   if (trap_from_user(frame))
   {
      handle_user_exception(frame);
      return;
   }

   handle_supervisor_exception(frame);
}

void trap_handler(TrapFrame *frame)
{
   if (trap_is_interrupt(frame->cause))
   {
      handle_interrupt(frame);
      return;
   }

   handle_exception(frame);
}