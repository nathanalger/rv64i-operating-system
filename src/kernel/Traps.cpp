#include "Traps.hpp"
#include "CSR.hpp"
#include "UART.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"
#include "TrapHandler.hpp"
#include "Debug.hpp"

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
   Debug::prints("Supervisor Trap Encountered\n");

   const bool is_interrupt = trap_is_interrupt(frame->cause);
   const bool from_user = ((frame->status & SSTATUS_SPP) == 0);

   if (is_interrupt)
      Debug::prints("Type: Interrupt\n");
   else
      Debug::prints("Type: Exception\n");

   Debug::prints("Origin: ");
   Debug::prints(from_user ? "User\n" : "Supervisor\n");

   Debug::prints("Code: ");
   Debug::print_hex(trap_code(frame->cause));
   Debug::prints("\n");

   Debug::prints("sepc: ");
   Debug::print_hex(frame->epc);
   Debug::prints("\n");

   Debug::prints("stval: ");
   Debug::print_hex(frame->tval);
   Debug::prints("\n");

   trap_handler(frame);
}

extern "C" void machine_trap_handler(TrapFrame *frame)
{
   Debug::prints("Machine trap encountered\n");

   Debug::prints("mcause: ");
   Debug::print_hex(frame->cause);
   Debug::prints("\n");

   Debug::prints("mepc: ");
   Debug::print_hex(frame->epc);
   Debug::prints("\n");

   Debug::prints("mtval: ");
   Debug::print_hex(frame->tval);
   Debug::prints("\n");

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