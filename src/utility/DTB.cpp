#include "utility/DTB.hpp"
#include "utility/PlatformInfo.hpp"
#include "panic.hpp"
extern "C"
{
#include <libfdt.h>
}

extern "C" char __bss_start[];
extern "C" char __bss_end[];
extern "C" char _kernel_end[];
extern "C" char _kernel_start[];

// Helpers

static uint32_t be32_to_cpu(uint32_t value)
{
    return ((value & 0x000000FFu) << 24) |
           ((value & 0x0000FF00u) << 8)  |
           ((value & 0x00FF0000u) >> 8)  |
           ((value & 0xFF000000u) >> 24);
}

struct DtbHeader
{
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

// End Helpers

static const void* g_dtb = nullptr;
static uint64_t read_cells(const fdt32_t* cells, int count)
{
    uint64_t value = 0;
    for (int i = 0; i < count; ++i)
    {
        value = (value << 32) | fdt32_to_cpu(cells[i]);
    }
    return value;
}
static int get_root_addr_cells()
{
    if (g_dtb == nullptr)
    {
        return -1;
    }
    int root = fdt_path_offset(g_dtb, "/");
    if (root < 0)
    {
        return -1;
    }
    int value = fdt_address_cells(g_dtb, root);
    return (value > 0) ? value : -1;
}
static int get_root_size_cells()
{
    if (g_dtb == nullptr)
    {
        return -1;
    }
    int root = fdt_path_offset(g_dtb, "/");
    if (root < 0)
    {
        return -1;
    }
    int value = fdt_size_cells(g_dtb, root);
    return (value > 0) ? value : -1;
}
bool dtb_init(const void* dtb)
{
    if (dtb == nullptr)
    {
        g_dtb = nullptr;
        return false;
    }
    if (fdt_check_header(dtb) != 0)
    {
        g_dtb = nullptr;
        return false;
    }
    g_dtb = dtb;
    return true;
}
const void* dtb_raw()
{
    return g_dtb;
}
bool dtb_get_memory_range(uint64_t& base_out, uint64_t& size_out)
{
    base_out = 0;
    size_out = 0;
    if (g_dtb == nullptr)
    {
        return false;
    }
    const int addr_cells = get_root_addr_cells();
    const int size_cells = get_root_size_cells();
    if (addr_cells <= 0 || size_cells <= 0)
    {
        return false;
    }
    int memory_node = -1;
    for (int node = fdt_next_node(g_dtb, -1, nullptr);
         node >= 0;
         node = fdt_next_node(g_dtb, node, nullptr))
    {
        int len = 0;
        const char* device_type =
            (const char*)fdt_getprop(g_dtb, node, "device_type", &len);
        if (device_type != nullptr && len >= 6)
        {
            if (device_type[0] == 'm' &&
                device_type[1] == 'e' &&
                device_type[2] == 'm' &&
                device_type[3] == 'o' &&
                device_type[4] == 'r' &&
                device_type[5] == 'y')
            {
                memory_node = node;
                break;
            }
        }
    }
    if (memory_node < 0)
    {
        return false;
    }
    int reg_len = 0;
    const fdt32_t* reg =
        (const fdt32_t*)fdt_getprop(g_dtb, memory_node, "reg", &reg_len);
    if (reg == nullptr)
    {
        return false;
    }
    const int entry_cells = addr_cells + size_cells;
    const int entry_bytes = entry_cells * (int)sizeof(fdt32_t);
    if (reg_len < entry_bytes)
    {
        return false;
    }
    base_out = read_cells(reg, addr_cells);
    size_out = read_cells(reg + addr_cells, size_cells);
    return true;
}
bool dtb_get_uart_base(uint64_t& uart_base_out)
{
    uart_base_out = 0;
    if (g_dtb == nullptr)
    {
        return false;
    }
    const int addr_cells = get_root_addr_cells();
    if (addr_cells <= 0)
    {
        return false;
    }
    for (int node = fdt_next_node(g_dtb, -1, nullptr);
         node >= 0;
         node = fdt_next_node(g_dtb, node, nullptr))
    {
        int compat_len = 0;
        const char* compatible =
            (const char*)fdt_getprop(g_dtb, node, "compatible", &compat_len);
        if (compatible == nullptr || compat_len <= 0)
        {
            continue;
        }
        bool looks_like_uart = false;
        int i = 0;
        while (i < compat_len)
        {
            const char* s = compatible + i;
            bool matched_ns16550 =
                s[0] == 'n' && s[1] == 's' && s[2] == '1' && s[3] == '6' &&
                s[4] == '5' && s[5] == '5' && s[6] == '0';
            bool matched_uart =
                s[0] == 'u' && s[1] == 'a' && s[2] == 'r' && s[3] == 't';
            if (matched_ns16550 || matched_uart)
            {
                looks_like_uart = true;
                break;
            }
            while (i < compat_len && compatible[i] != '\0')
            {
                ++i;
            }
            ++i;
        }
        if (!looks_like_uart)
        {
            continue;
        }
        int reg_len = 0;
        const fdt32_t* reg =
            (const fdt32_t*)fdt_getprop(g_dtb, node, "reg", &reg_len);
        if (reg == nullptr)
        {
            continue;
        }
        if (reg_len < addr_cells * (int)sizeof(fdt32_t))
        {
            continue;
        }
        uart_base_out = read_cells(reg, addr_cells);
        return true;
    }
    return false;
}
bool platform_info_init(PlatformInfo& platform, const void* dtb)
{
    platform.dtb_addr = (uint64_t)dtb;

    const DtbHeader* header = reinterpret_cast<const DtbHeader*>(dtb);
    platform.dtb_size = be32_to_cpu(header->totalsize);
    platform.dtb_end = platform.dtb_addr + platform.dtb_size;

    if (!dtb_init(dtb))
        return false;

    if (!dtb_get_uart_base(platform.uart_base))
        return false;

    if (!dtb_get_memory_range(platform.ram_base, platform.ram_size))
        return false;

    platform.kernel_start = (uint64_t)_kernel_start;
    platform.kernel_end = (uint64_t)_kernel_end;
    platform.bss_start = (uint64_t)__bss_start;
    platform.bss_end = (uint64_t)__bss_end;

    return true;
}