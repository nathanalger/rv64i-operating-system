#include "ProcessLoader.hpp"
#include "Constants.hpp"
#include "ProcessMemory.hpp"
#include "CSR.hpp"

extern "C" unsigned char _binary_build_user_init_bin_start[];
extern "C" unsigned char _binary_build_user_init_bin_end[];

bool process_load_initial_user_program(PageTable *root,
                                       PhysicalPageAllocator &allocator,
                                       LoadedUserProgram &program)
{
   if (!root)
      return false;

   const uint8_t *image_src = _binary_build_user_init_bin_start;
   const uint64_t image_size =
       (uint64_t)(_binary_build_user_init_bin_end - _binary_build_user_init_bin_start);

   ProcessMemoryLayout layout = {};
   if (!process_memory_layout_init(layout,
                                   image_size,
                                   0,
                                   0,
                                   PAGE_SIZE,
                                   USER_STACK_SIZE))
   {
      return false;
   }

   if (!map_region(root, layout.text, allocator))
      return false;

   if (!map_region(root, layout.bss, allocator))
      return false;

   if (!map_region(root, layout.stack, allocator))
      return false;

   if (!copy_into_region(root, layout.text.start, image_src, image_size))
      return false;

   sfence_vma();

   program.entry = layout.entry;
   program.stack_top = layout.stack_top;
   return true;
}