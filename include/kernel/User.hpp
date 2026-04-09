#pragma once
#include <stdint.h>
#include <stddef.h>

struct alignas(8) UserContext
{
   uint64_t ra;
   uint64_t sp;
   uint64_t gp;
   uint64_t tp;
   uint64_t t0;
   uint64_t t1;
   uint64_t t2;
   uint64_t s0;
   uint64_t s1;
   uint64_t a0;
   uint64_t a1;
   uint64_t a2;
   uint64_t a3;
   uint64_t a4;
   uint64_t a5;
   uint64_t a6;
   uint64_t a7;
   uint64_t s2;
   uint64_t s3;
   uint64_t s4;
   uint64_t s5;
   uint64_t s6;
   uint64_t s7;
   uint64_t s8;
   uint64_t s9;
   uint64_t s10;
   uint64_t s11;
   uint64_t t3;
   uint64_t t4;
   uint64_t t5;
   uint64_t t6;

   uint64_t epc;
   uint64_t status;
};

static_assert(sizeof(UserContext) == 264, "UserContext size mismatch");
static_assert(offsetof(UserContext, epc) == 248, "UserContext epc offset mismatch");
static_assert(offsetof(UserContext, status) == 256, "UserContext status offset mismatch");

struct PageTable;
struct PhysicalPageAllocator;

constexpr uint64_t USER_CODE_VIRTUAL_BASE = 0x0000000000400000ULL;
constexpr uint64_t USER_STACK_VIRTUAL_BASE = 0x0000000000410000ULL;
constexpr uint64_t USER_STACK_SIZE = 4096;

extern "C" void user_enter(UserContext *context);

void user_context_init(UserContext &context,
                       uint64_t entry,
                       uint64_t stack_top,
                       uint64_t arg0,
                       uint64_t arg1);

[[noreturn]] void enter_user_mode(UserContext &context);

void write_initial_user_program(uint64_t user_code_physical_base);

bool user_address_space_init(PageTable *root,
                             PhysicalPageAllocator &allocator,
                             uint64_t &user_entry_out,
                             uint64_t &user_stack_top_out);

[[noreturn]] void launch_first_user_task(PageTable *root,
                                         PhysicalPageAllocator &allocator,
                                         uint64_t hartid,
                                         uint64_t dtb);