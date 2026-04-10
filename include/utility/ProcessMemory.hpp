#pragma once

#include <cstdint>
#include "Paging.hpp"
#include "PhysicalPageAllocator.hpp"

struct MemoryRegion
{
   uint64_t start;
   uint64_t size;
   uint64_t flags;
};

struct ProcessMemoryLayout
{
   MemoryRegion text;
   MemoryRegion rodata;
   MemoryRegion data;
   MemoryRegion bss;
   MemoryRegion heap;
   MemoryRegion stack;

   uint64_t heap_break;
   uint64_t stack_top;
   uint64_t entry;
};

inline uint64_t region_end(const MemoryRegion &region)
{
   return region.start + region.size;
}

bool map_region(PageTable *root,
                const MemoryRegion &region,
                PhysicalPageAllocator &allocator);

bool process_memory_layout_init(ProcessMemoryLayout &layout,
                                uint64_t text_size,
                                uint64_t rodata_size,
                                uint64_t data_size,
                                uint64_t bss_size,
                                uint64_t stack_size);

bool copy_into_region(PageTable *root,
                      uint64_t virtual_address,
                      const uint8_t *source,
                      uint64_t size);
