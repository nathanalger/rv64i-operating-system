#include <stdint.h>
#include "UART.hpp"
#include "panic.hpp"
#include "PlatformInfo.hpp"
#include "Paging.hpp"
#include "PrintHex.hpp"
#include "CSR.hpp"
#include "Traps.hpp"
#include "PhysicalPageAllocator.hpp"

extern "C" void supervisor_main(uint64_t hartid, const void *dtb);

extern "C" void machine_main(uint64_t hartid, const void *dtb)
{
   machine_traps_init();
   delegate_supervisor_exceptions();
   pmp_init_allow_all();
   enter_supervisor_mode(hartid, dtb);

   panic("Returned from enter_supervisor_mode unexpectedly.");
}