#include <stdint.h>
#include "UART.hpp"
#include "printHex.hpp"
#include "DTB.hpp"
#include "panic.hpp"
#include "PlatformInfo.hpp"

extern "C" void kernel_main(const void* dtb)
{
    PlatformInfo _platform_info = {};
    
    if(!(platform_info_init(_platform_info, dtb))) {
        panic("Failed to initialize platform information.");
    }

    panic("Reached end of execution!");
}