#include "CSR.hpp"
#include "UART.hpp"
#include "Traps.hpp"
#include "PagingTests.hpp"
#include "Panic.hpp"
#include "PrintHex.hpp"
#include "DTB.hpp"
#include "User.hpp"
#include "Debug.hpp"

extern "C" void supervisor_main(uint64_t hartid, const void *dtb)
{
   (void)hartid;

   PlatformInfo platform = {};
   if (!platform_info_init(platform, dtb))
      panic("Failed to initialize platform information.");

   PhysicalPageAllocator allocator = {};
   if (!physical_page_allocator_init(allocator, platform))
      panic("Failed to initialize physical page allocator.");

   PageTable *root = nullptr;
   if (!paging_init(root, allocator, platform))
      panic("Failed to initialize paging.");

   supervisor_traps_init();

   Debug::prints("Supervisor Mode Reached\n");

   // Prepare for user mode entry
   launch_first_user_task(root, allocator, hartid, (uint64_t)dtb);

   panic("Unexpectedly reached end of supervisor mode execution.");
}