#include "CSR.hpp"
#include "UART.hpp"
#include "Traps.hpp"

extern "C" void supervisor_main()
{
   uart_puts("Supervisor Mode Reached\n");
   supervisor_traps_init();

   while (1)
   {
   }
}