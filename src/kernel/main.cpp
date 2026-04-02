#include <stdint.h>
#include "UART.hpp"
#include "DTB.hpp"
#include "panic.hpp"
#include "PlatformInfo.hpp"
#include "Paging.hpp"
#include "PrintHex.hpp"
#include "CSR.hpp"
#include "PhysicalPageAllocator.hpp"

extern "C" void kernel_main(const void *dtb)
{
    PlatformInfo platform = {};
    if (!platform_info_init(platform, dtb))
        panic("Failed to initialize platform information.");

    PhysicalPageAllocator allocator = {};
    if (!physical_page_allocator_init(allocator, platform))
        panic("Failed to initialize physical page allocator.");

    PageTable *root = nullptr;
    if (!paging_init(root, allocator, platform))
        panic("Failed to initialize paging.");

    panic("Done.");
}