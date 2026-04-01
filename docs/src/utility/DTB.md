# DTB Utility

This is a wrapper for the libfdt third-party libfdt library. It is used to read the device tree from memory and parse it. 


## DTB Init

Initialize the DTB utility with the raw pointer passed to the kernel.
Returns true if the blob looks valid.

```cpp
bool dtb_init(const void* dtb);
```

## DTB Raw

Returns the raw DTB pointer currently stored by the utility.

```cpp
const void* dtb_raw();
```

## DTB Get Memory Range

Read the first memory region from the device tree.

On success:
  - base_out receives the start address
  - size_out receives the size in bytes

Returns true on success.

```cpp
bool dtb_get_memory_range(uint64_t& base_out, uint64_t& size_out);
```

## DTB Get UART Base

Read the UART base address from the "chosen" compatible serial node.
Returns true on success.

```cpp
bool dtb_get_uart_base(uint64_t& uart_base_out);
```