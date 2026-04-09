#pragma once
#include <stdint.h>
#include <stddef.h>
#include "PlatformInfo.hpp"

struct PhysicalPageAllocator
{
    uint64_t current;
    uint64_t end;
};

bool physical_page_allocator_init(PhysicalPageAllocator &allocator,
                                  const PlatformInfo &platform);

uint64_t physical_alloc_page(PhysicalPageAllocator &allocator);

uint64_t align_up(uint64_t value, uint64_t alignment);