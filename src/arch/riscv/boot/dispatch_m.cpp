#include <stdint.h>
#include "Panic.hpp"

extern "C" void machine_main(uint64_t hartid, const void *dtb);

extern "C" void dispatch(uint64_t hartid, const void *dtb)
{
   machine_main(hartid, dtb);
   panic("dispatch returned unexpectedly");
}