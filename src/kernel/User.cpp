#include "User.hpp"
#include "CSR.hpp"
#include "Panic.hpp"
#include "PhysicalPageAllocator.hpp"
#include "Debug.hpp"
#include "ProcessLoader.hpp"

static inline uint64_t align_down(uint64_t value, uint64_t alignment)
{
   return value & ~(alignment - 1);
}

void user_context_init(UserContext &context,
                       uint64_t entry,
                       uint64_t stack_top,
                       uint64_t arg0,
                       uint64_t arg1)
{
   context.ra = 0;
   context.sp = align_down(stack_top, 16);
   context.gp = 0;
   context.tp = 0;
   context.t0 = 0;
   context.t1 = 0;
   context.t2 = 0;
   context.s0 = 0;
   context.s1 = 0;
   context.a0 = arg0;
   context.a1 = arg1;
   context.a2 = 0;
   context.a3 = 0;
   context.a4 = 0;
   context.a5 = 0;
   context.a6 = 0;
   context.a7 = 0;
   context.s2 = 0;
   context.s3 = 0;
   context.s4 = 0;
   context.s5 = 0;
   context.s6 = 0;
   context.s7 = 0;
   context.s8 = 0;
   context.s9 = 0;
   context.s10 = 0;
   context.s11 = 0;
   context.t3 = 0;
   context.t4 = 0;
   context.t5 = 0;
   context.t6 = 0;

   context.epc = entry;

   uint64_t status = csr_read_sstatus();
   status &= ~SSTATUS_SPP;
   status |= SSTATUS_SPIE;
   status &= ~SSTATUS_SIE;

   context.status = status;
}

[[noreturn]] void enter_user_mode(UserContext &context)
{
   Debug::prints("Entering user mode\n");
   Debug::prints("  epc: ");
   Debug::print_hex(context.epc);
   Debug::prints("\n");

   Debug::prints("  sp : ");
   Debug::print_hex(context.sp);
   Debug::prints("\n");

   user_enter(&context);

   panic("Returned from user_enter unexpectedly.");
}

[[noreturn]] void user_task_init(PageTable *root,
                                 PhysicalPageAllocator &allocator,
                                 uint64_t hartid,
                                 uint64_t dtb)
{
   LoadedUserProgram program = {};

   if (!process_load_initial_user_program(root, allocator, program))
      panic("Failed to load initial user program.");

   Debug::prints("User code VA : ");
   Debug::print_hex(program.entry);
   Debug::prints("\n");

   Debug::prints("User stack VA: ");
   Debug::print_hex(program.stack_top);
   Debug::prints("\n");

   UserContext context = {};
   user_context_init(context,
                     program.entry,
                     program.stack_top,
                     hartid,
                     dtb);

   enter_user_mode(context);
}