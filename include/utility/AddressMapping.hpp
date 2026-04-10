#pragma once
#include <stdint.h>

constexpr uint64_t KERNEL_DIRECT_MAP_BASE = 0xFFFFFFC000000000ULL;

inline uint64_t phys_to_virt(uint64_t physical_address)
{
   return physical_address + KERNEL_DIRECT_MAP_BASE;
}

inline uint64_t virt_to_phys(uint64_t virtual_address)
{
   return virtual_address - KERNEL_DIRECT_MAP_BASE;
}

template <typename T>
inline T *phys_to_virt_ptr(uint64_t physical_address)
{
   return reinterpret_cast<T *>(phys_to_virt(physical_address));
}

template <typename T>
inline T *phys_to_ptr_early(uint64_t physical_address)
{
   return reinterpret_cast<T *>(physical_address);
}