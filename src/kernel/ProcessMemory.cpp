#include "ProcessMemory.hpp"
#include "Paging.hpp"
#include "User.hpp"
#include "AddressMapping.hpp"
#include "Constants.hpp"

static inline uint64_t align_down(uint64_t value, uint64_t alignment)
{
   return value & ~(alignment - 1);
}

bool process_memory_layout_init(ProcessMemoryLayout &layout,
                                uint64_t text_size,
                                uint64_t rodata_size,
                                uint64_t data_size,
                                uint64_t bss_size,
                                uint64_t stack_size)
{
   text_size = align_up(text_size, PAGE_SIZE);
   rodata_size = align_up(rodata_size, PAGE_SIZE);
   data_size = align_up(data_size, PAGE_SIZE);
   bss_size = align_up(bss_size, PAGE_SIZE);
   stack_size = align_up(stack_size, PAGE_SIZE);

   if (stack_size == 0)
      stack_size = PAGE_SIZE;

   const uint64_t text_start = USER_TEXT_START;
   const uint64_t rodata_start = text_start + text_size;
   const uint64_t data_start = rodata_start + rodata_size;
   const uint64_t bss_start = data_start + data_size;
   const uint64_t heap_start = bss_start + bss_size;

   const uint64_t stack_top = align_down(USER_STACK_TOP, PAGE_SIZE);
   const uint64_t stack_start = stack_top - stack_size;

   if (stack_start <= heap_start)
      return false;

   layout.text.flags = PTE_R | PTE_X | PTE_U;
   layout.rodata.flags = PTE_R | PTE_U;
   layout.data.flags = PTE_R | PTE_W | PTE_U;
   layout.bss.flags = PTE_R | PTE_W | PTE_U;
   layout.heap.flags = PTE_R | PTE_W | PTE_U;
   layout.stack.flags = PTE_R | PTE_W | PTE_U;

   layout.text.start = text_start;
   layout.text.size = text_size;

   layout.rodata.start = rodata_start;
   layout.rodata.size = rodata_size;

   layout.data.start = data_start;
   layout.data.size = data_size;

   layout.bss.start = bss_start;
   layout.bss.size = bss_size;

   layout.heap.start = heap_start;
   layout.heap.size = 0;

   layout.stack.start = stack_start;
   layout.stack.size = stack_size;

   layout.heap_break = heap_start;
   layout.stack_top = stack_top;
   layout.entry = text_start;

   return true;
}

bool map_region(PageTable *root,
                const MemoryRegion &region,
                PhysicalPageAllocator &allocator)
{
   if (!root)
      return false;

   if ((region.start & (PAGE_SIZE - 1)) != 0)
      return false;

   if ((region.size & (PAGE_SIZE - 1)) != 0)
      return false;

   uint64_t offset = 0;
   while (offset < region.size)
   {
      const uint64_t physical_page = physical_alloc_page(allocator);
      if (physical_page == 0)
         return false;

      uint8_t *page_bytes = phys_to_virt_ptr<uint8_t>(physical_page);
      for (uint64_t i = 0; i < PAGE_SIZE; ++i)
         page_bytes[i] = 0;

      if (!paging_map(root,
                      region.start + offset,
                      physical_page,
                      region.flags,
                      allocator))
      {
         return false;
      }

      offset += PAGE_SIZE;
   }

   return true;
}

bool copy_into_region(PageTable *root,
                      uint64_t virtual_address,
                      const uint8_t *source,
                      uint64_t size)
{
   if (!root && size != 0)
      return false;

   for (uint64_t i = 0; i < size; ++i)
   {
      uint64_t physical_address = 0;
      uint64_t flags = 0;

      if (!paging_query(root, virtual_address + i, physical_address, flags))
         return false;

      uint8_t *dst = phys_to_virt_ptr<uint8_t>(physical_address);
      *dst = source[i];
   }

   return true;
}