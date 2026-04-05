#include <stdint.h>
#include "UART.hpp"
#include "DTB.hpp"
#include "panic.hpp"
#include "PlatformInfo.hpp"
#include "Paging.hpp"
#include "PrintHex.hpp"
#include "CSR.hpp"
#include "Traps.hpp"
#include "PhysicalPageAllocator.hpp"

extern "C" void machine_main(const void *dtb);
extern "C" void supervisor_main(const void *dtb);

extern "C" void dispatch(const void *dtb)
{
    machine_main(dtb);

    panic("dispatch returned unexpectedly");
}