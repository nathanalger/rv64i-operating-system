#include "User.hpp"
#include "CSR.hpp"
#include "UART.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"
#include "Paging.hpp"
#include "PhysicalPageAllocator.hpp"
#include "AddressSpace.hpp"

static inline uint64_t align_down(uint64_t value, uint64_t alignment)
{
   return value & ~(alignment - 1);
}

static void zero_page(uint64_t physical_address)
{
   uint8_t *bytes = phys_to_virt_ptr<uint8_t>(physical_address);

   for (uint64_t i = 0; i < PAGE_SIZE; ++i)
      bytes[i] = 0;
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
   uart_puts("Entering user mode\n");
   uart_puts("  epc: ");
   Utility::print_hex(context.epc);
   uart_puts("\n");

   uart_puts("  sp : ");
   Utility::print_hex(context.sp);
   uart_puts("\n");

   user_enter(&context);

   panic("Returned from user_enter unexpectedly.");
}

void write_initial_user_program(uint64_t user_code_physical_base)
{
   volatile uint32_t *code =
       phys_to_virt_ptr<volatile uint32_t>(user_code_physical_base);

   code[0] = 0x00000073; // ecall
   code[1] = 0x0000006F; // jal x0, 0
}

bool user_address_space_init(PageTable *root,
                             PhysicalPageAllocator &allocator,
                             uint64_t &user_entry_out,
                             uint64_t &user_stack_top_out)
{
   if (!root)
      return false;

   const uint64_t user_code_phys = physical_alloc_page(allocator);
   if (user_code_phys == 0)
      return false;

   const uint64_t user_stack_phys = physical_alloc_page(allocator);
   if (user_stack_phys == 0)
      return false;

   zero_page(user_code_phys);
   zero_page(user_stack_phys);

   write_initial_user_program(user_code_phys);

   if (!paging_map(root,
                   USER_CODE_VIRTUAL_BASE,
                   user_code_phys,
                   PTE_R | PTE_X | PTE_U,
                   allocator))
   {
      return false;
   }

   if (!paging_map(root,
                   USER_STACK_VIRTUAL_BASE,
                   user_stack_phys,
                   PTE_R | PTE_W | PTE_U,
                   allocator))
   {
      return false;
   }

   sfence_vma();

   user_entry_out = USER_CODE_VIRTUAL_BASE;
   user_stack_top_out = USER_STACK_VIRTUAL_BASE + USER_STACK_SIZE;
   return true;
}

[[noreturn]] void launch_first_user_task(PageTable *root,
                                         PhysicalPageAllocator &allocator,
                                         uint64_t hartid,
                                         uint64_t dtb)
{
   uint64_t user_entry = 0;
   uint64_t user_stack_top = 0;

   if (!user_address_space_init(root, allocator, user_entry, user_stack_top))
      panic("Failed to initialize user address space.");

   uart_puts("User code VA : ");
   Utility::print_hex(user_entry);
   uart_puts("\n");

   uart_puts("User stack VA: ");
   Utility::print_hex(user_stack_top);
   uart_puts("\n");

   UserContext context = {};
   user_context_init(context,
                     user_entry,
                     user_stack_top,
                     hartid,
                     dtb);

   enter_user_mode(context);
}