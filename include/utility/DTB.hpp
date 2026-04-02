#pragma once

#include <stdint.h>
#include "PlatformInfo.hpp"

/**
 * Initialize the DTB utility with the raw pointer passed to the kernel.
 *
 * Returns true if the blob looks valid.
 */
bool dtb_init(const void* dtb);
/**
 * Returns the raw DTB pointer currently stored by the utility.
 */
const void* dtb_raw();
/**
 * Read the first memory region from the device tree.
 *
 * On success:
 *   - base_out receives the start address
 *   - size_out receives the size in bytes
 *
 * Returns true on success.
 */
bool dtb_get_memory_range(uint64_t& base_out, uint64_t& size_out);
/**
 * Read the UART base address from the "chosen" compatible serial node.
 *
 * This is optional, but useful later if you want to stop hardcoding UART.
 *
 * Returns true on success.
 */
bool dtb_get_uart_base(uint64_t& uart_base_out);
/**
 * Initialize the platform information from the device tree and load it into 
 * platform parameter. Returns true on success, false on failure.
 */
bool platform_info_init(PlatformInfo& platform, const void* dtb);