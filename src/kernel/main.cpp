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

    traps_init();

    uart_puts("Before page fault\n");
    *(volatile uint64_t *)0x12345000ULL = 1;
    uart_puts("After page fault\n");

    panic("Done.");
}