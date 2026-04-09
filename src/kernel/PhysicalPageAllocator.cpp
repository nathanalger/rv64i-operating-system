#include "PhysicalPageAllocator.hpp"

uint64_t align_up(uint64_t value, uint64_t alignment)
{
   return (value + alignment - 1) & ~(alignment - 1);
}

static uint64_t max_u64(uint64_t a, uint64_t b)
{
   return (a > b) ? a : b;
}

bool physical_page_allocator_init(PhysicalPageAllocator &allocator,
                                  const PlatformInfo &platform)
{
   const uint64_t dtb_end = platform.dtb_addr + platform.dtb_size;
   const uint64_t reserved_end = max_u64(platform.kernel_end, dtb_end);

   allocator.current = align_up(reserved_end, 4096);
   allocator.end = platform.ram_base + platform.ram_size;

   if (allocator.current >= allocator.end)
      return false;

   return true;
}

uint64_t physical_alloc_page(PhysicalPageAllocator &allocator)
{
   const uint64_t page_size = 4096;

   uint64_t addr = align_up(allocator.current, page_size);

   if (addr + page_size > allocator.end)
      return 0;

   allocator.current = addr + page_size;
   return addr;
}