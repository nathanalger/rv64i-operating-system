#include "Paging.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"
#include "UART.hpp"

extern "C" char _text_start[];
extern "C" char _rodata_start[];
extern "C" char _data_start[];

namespace Utility
{
   extern "C" char _text_start[];
   extern "C" char _rodata_start[];
   extern "C" char _data_start[];

   using Func = void (*)();

   inline static void test_page_walker(PageTable *root)
   {
      auto test_va = [&](const char *name, uint64_t va)
      {
         uint64_t pa = 0;
         uint64_t flags = 0;

         uart_puts(name);
         uart_puts(" VA: ");
         Utility::print_hex(va);

         if (!paging_query(root, va, pa, flags))
         {
            uart_puts(" -> unmapped\n");
            return;
         }

         uart_puts(" -> PA: ");
         Utility::print_hex(pa);

         uart_puts(" flags=");

         uart_puts((flags & PTE_V) ? "V" : "-");
         uart_puts((flags & PTE_R) ? "R" : "-");
         uart_puts((flags & PTE_W) ? "W" : "-");
         uart_puts((flags & PTE_X) ? "X" : "-");

         uart_puts("\n");
      };

      test_va("TEXT   ", (uint64_t)_text_start);
      test_va("RODATA ", (uint64_t)_rodata_start);
      test_va("DATA   ", (uint64_t)_data_start);
   }

   inline static void test_text_write_fault()
   {
      volatile uint64_t *p = reinterpret_cast<volatile uint64_t *>(_text_start);
      *p = 0xDEADBEEFCAFEBABEULL;
   }

   inline static void test_rodata_write_fault()
   {
      volatile uint64_t *p = reinterpret_cast<volatile uint64_t *>(_rodata_start);
      *p = 0x1122334455667788ULL;
   }

   inline static void test_data_exec_fault()
   {
      Func fn = reinterpret_cast<Func>(_data_start);
      fn();
   }
}