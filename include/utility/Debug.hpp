#include "UART.hpp"
#include "PrintHex.hpp"

namespace Debug
{
   static bool debug = false;

   static void prints(const char *str)
   {
      if (debug)
         uart_puts(str);
   }

   static void print_hex(uint64_t val)
   {
      if (debug)
         Utility::print_hex(val);
   }
}