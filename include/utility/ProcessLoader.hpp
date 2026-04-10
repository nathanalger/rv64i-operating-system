#pragma once
#include "Paging.hpp"
#include "PhysicalPageAllocator.hpp"

struct LoadedUserProgram
{
   uint64_t entry;
   uint64_t stack_top;
};

bool process_load_initial_user_program(PageTable *root,
                                       PhysicalPageAllocator &allocator,
                                       LoadedUserProgram &program);