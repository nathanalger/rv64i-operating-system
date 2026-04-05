#include "Paging.hpp"
#include "DTB.hpp"
#include "CSR.hpp"
#include "PagingTests.hpp"

extern "C" char __bss_start[];
extern "C" char __bss_end[];

extern "C" char _text_start[];
extern "C" char _text_end[];

extern "C" char _rodata_start[];
extern "C" char _rodata_end[];

extern "C" char _data_start[];
extern "C" char _data_end[];

PageTable *g_kernel_root = nullptr;

uint64_t sv39_vpn2(uint64_t virtual_address)
{
   return (virtual_address >> 30) & 0x1FF;
}

uint64_t sv39_vpn1(uint64_t virtual_address)
{
   return (virtual_address >> 21) & 0x1FF;
}

uint64_t sv39_vpn0(uint64_t virtual_address)
{
   return (virtual_address >> 12) & 0x1FF;
}

uint64_t pte_from_phys_addr(uint64_t physical_address, uint64_t flags)
{
   uint64_t ppn = physical_address >> 12;
   return (ppn << 10) | flags;
}

uint64_t phys_addr_from_pte(uint64_t pte)
{
   uint64_t ppn = (pte >> 10) & 0xFFFFFFFFFFFULL;
   return ppn << 12;
}

void paging_zero_table(PageTable *table)
{
   for (uint64_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
   {
      table->entries[i] = 0;
   }
}

PageTable *paging_create_root_table(PhysicalPageAllocator &allocator)
{
   void *page = physical_alloc_page(allocator);
   if (!page)
      return nullptr;

   PageTable *table = reinterpret_cast<PageTable *>(page);
   paging_zero_table(table);
   return table;
}

bool pte_is_valid(uint64_t pte)
{
   return (pte & PTE_V) != 0;
}

bool pte_is_leaf(uint64_t pte)
{
   return (pte & (PTE_R | PTE_W | PTE_X)) != 0;
}

bool paging_map(PageTable *root,
                uint64_t virtual_address,
                uint64_t physical_address,
                uint64_t flags,
                PhysicalPageAllocator &allocator)
{
   if (!root)
      return false;

   if ((virtual_address & (PAGE_SIZE - 1)) != 0)
      return false;

   if ((physical_address & (PAGE_SIZE - 1)) != 0)
      return false;

   const uint64_t vpn2 = sv39_vpn2(virtual_address);
   const uint64_t vpn1 = sv39_vpn1(virtual_address);
   const uint64_t vpn0 = sv39_vpn0(virtual_address);

   PageTable *level1 = nullptr;
   PageTable *level0 = nullptr;

   uint64_t &l2_entry = root->entries[vpn2];
   if (!pte_is_valid(l2_entry))
   {
      void *new_page = physical_alloc_page(allocator);
      if (!new_page)
         return false;

      level1 = reinterpret_cast<PageTable *>(new_page);
      paging_zero_table(level1);

      l2_entry = pte_from_phys_addr((uint64_t)level1, PTE_V);
   }
   else
   {
      if (pte_is_leaf(l2_entry))
         return false;

      level1 = reinterpret_cast<PageTable *>(phys_addr_from_pte(l2_entry));
   }

   uint64_t &l1_entry = level1->entries[vpn1];
   if (!pte_is_valid(l1_entry))
   {
      void *new_page = physical_alloc_page(allocator);
      if (!new_page)
         return false;

      level0 = reinterpret_cast<PageTable *>(new_page);
      paging_zero_table(level0);

      l1_entry = pte_from_phys_addr((uint64_t)level0, PTE_V);
   }
   else
   {
      if (pte_is_leaf(l1_entry))
         return false;

      level0 = reinterpret_cast<PageTable *>(phys_addr_from_pte(l1_entry));
   }

   uint64_t &l0_entry = level0->entries[vpn0];

   if (pte_is_valid(l0_entry))
      return false;

   l0_entry = pte_from_phys_addr(physical_address, flags | PTE_V);

   return true;
}

bool paging_map_range(PageTable *root,
                      uint64_t virtual_start,
                      uint64_t physical_start,
                      uint64_t size,
                      uint64_t flags,
                      PhysicalPageAllocator &allocator)
{
   if (size == 0)
      return true;

   uint64_t mapped = 0;
   uint64_t page_aligned_size = align_up(size, PAGE_SIZE);

   while (mapped < page_aligned_size)
   {
      if (!paging_map(root,
                      virtual_start + mapped,
                      physical_start + mapped,
                      flags,
                      allocator))
      {
         return false;
      }

      mapped += PAGE_SIZE;
   }

   return true;
}

bool paging_init(PageTable *&root,
                 PhysicalPageAllocator &allocator,
                 const PlatformInfo &platform)
{
   root = paging_create_root_table(allocator);
   if (!root)
      return false;

   if (!paging_map_range(root,
                         (uint64_t)_text_start,
                         (uint64_t)_text_start,
                         (uint64_t)_text_end - (uint64_t)_text_start,
                         PTE_R | PTE_X,
                         allocator))
   {
      return false;
   }

   if (!paging_map_range(root,
                         (uint64_t)_rodata_start,
                         (uint64_t)_rodata_start,
                         (uint64_t)_rodata_end - (uint64_t)_rodata_start,
                         PTE_R,
                         allocator))
   {
      return false;
   }

   if (!paging_map_range(root,
                         (uint64_t)_data_start,
                         (uint64_t)_data_start,
                         (uint64_t)_data_end - (uint64_t)_data_start,
                         PTE_R | PTE_W,
                         allocator))
   {
      return false;
   }

   if (!paging_map_range(root,
                         (uint64_t)__bss_start,
                         (uint64_t)__bss_start,
                         (uint64_t)__bss_end - (uint64_t)__bss_start,
                         PTE_R | PTE_W,
                         allocator))
   {
      return false;
   }

   uint64_t uart_page = platform.uart_base & ~(PAGE_SIZE - 1);

   if (!paging_map_range(root,
                         uart_page,
                         uart_page,
                         PAGE_SIZE,
                         PTE_R | PTE_W,
                         allocator))
   {
      return false;
   }

   if (!paging_map_range(root,
                         platform.dtb_addr,
                         platform.dtb_addr,
                         platform.dtb_size,
                         PTE_R | PTE_W,
                         allocator))
   {
      return false;
   }

   Utility::test_page_walker(root);

   uint64_t satp_value = make_satp_sv39((uint64_t)root);
   csr_write_satp(satp_value);
   sfence_vma();

   g_kernel_root = root;

   return true;
}

bool paging_query(PageTable *root, uint64_t va, uint64_t &pa, uint64_t &flags)
{
   pa = 0;
   flags = 0;

   if (!root)
      return false;

   const uint64_t vpn2 = sv39_vpn2(va);
   const uint64_t vpn1 = sv39_vpn1(va);
   const uint64_t vpn0 = sv39_vpn0(va);
   const uint64_t page_offset = va & (PAGE_SIZE - 1);

   const uint64_t l2_entry = root->entries[vpn2];
   if (!pte_is_valid(l2_entry) || pte_is_leaf(l2_entry))
      return false;

   PageTable *level1 = reinterpret_cast<PageTable *>(phys_addr_from_pte(l2_entry));

   const uint64_t l1_entry = level1->entries[vpn1];
   if (!pte_is_valid(l1_entry) || pte_is_leaf(l1_entry))
      return false;

   PageTable *level0 = reinterpret_cast<PageTable *>(phys_addr_from_pte(l1_entry));

   const uint64_t l0_entry = level0->entries[vpn0];
   if (!pte_is_valid(l0_entry))
      return false;

   if (!pte_is_leaf(l0_entry))
      return false;

   const uint64_t phys_page = phys_addr_from_pte(l0_entry);

   pa = phys_page + page_offset;
   flags = l0_entry & 0x3FFULL;
   return true;
}