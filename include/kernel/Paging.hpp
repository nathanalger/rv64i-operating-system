#pragma once
#include <stdint.h>
#include "PhysicalPageAllocator.hpp"

constexpr uint64_t PAGE_SIZE = 4096;
constexpr uint64_t PAGE_TABLE_ENTRIES = 512;

using PageTableEntry = uint64_t;

struct PageTable
{
   PageTableEntry entries[PAGE_TABLE_ENTRIES];
};

constexpr uint64_t PTE_V = 1ULL << 0;
constexpr uint64_t PTE_R = 1ULL << 1;
constexpr uint64_t PTE_W = 1ULL << 2;
constexpr uint64_t PTE_X = 1ULL << 3;
constexpr uint64_t PTE_U = 1ULL << 4;
constexpr uint64_t PTE_G = 1ULL << 5;
constexpr uint64_t PTE_A = 1ULL << 6;
constexpr uint64_t PTE_D = 1ULL << 7;

uint64_t sv39_vpn2(uint64_t virtual_address);
uint64_t sv39_vpn1(uint64_t virtual_address);
uint64_t sv39_vpn0(uint64_t virtual_address);

bool pte_is_valid(uint64_t pte);
bool pte_is_leaf(uint64_t pte);

bool paging_map(PageTable *root,
                uint64_t virtual_address,
                uint64_t physical_address,
                uint64_t flags,
                PhysicalPageAllocator &allocator);

bool paging_map_range(PageTable *root,
                      uint64_t virtual_start,
                      uint64_t physical_start,
                      uint64_t size,
                      uint64_t flags,
                      PhysicalPageAllocator &allocator);

bool paging_init(PageTable *&root,
                 PhysicalPageAllocator &allocator,
                 const PlatformInfo &platform);

uint64_t pte_from_phys_addr(uint64_t physical_address, uint64_t flags);
uint64_t phys_addr_from_pte(uint64_t pte);

PageTable *paging_create_root_table(PhysicalPageAllocator &allocator);
void paging_zero_table(PageTable *table);

bool paging_query(PageTable *root, uint64_t va, uint64_t &pa, uint64_t &flags);