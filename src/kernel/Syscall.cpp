#include "Syscall.hpp"
#include "Traps.hpp"
#include "UART.hpp"
#include "CSR.hpp"
#include "Panic.hpp"
#include <stdint.h>

namespace
{
   constexpr uint64_t SYSCALL_WRITE = 64;
   constexpr uint64_t SYSCALL_EXIT = 93;

   constexpr int64_t ERR_BADF = -9;
   constexpr int64_t ERR_FAULT = -14;
   constexpr int64_t ERR_NOSYS = -38;

   constexpr uint64_t SSTATUS_SUM = 1ULL << 18;

   static bool copy_from_user(void *dst, uint64_t user_src, uint64_t size)
   {
      if (dst == nullptr)
         return false;

      if (user_src == 0 && size != 0)
         return false;

      uint64_t old_status = csr_read_sstatus();
      csr_write_sstatus(old_status | SSTATUS_SUM);

      uint8_t *out = static_cast<uint8_t *>(dst);
      const volatile uint8_t *in = reinterpret_cast<const volatile uint8_t *>(user_src);

      for (uint64_t i = 0; i < size; ++i)
         out[i] = in[i];

      csr_write_sstatus(old_status);
      return true;
   }

   static int64_t syscall_write(uint64_t fd, uint64_t buffer, uint64_t length)
   {
      if (fd != 1 && fd != 2)
         return ERR_BADF;

      if (length == 0)
         return 0;

      const uint64_t max_chunk = 256;
      uint8_t temp[max_chunk];

      uint64_t total_written = 0;
      uint64_t remaining = length;
      uint64_t current_user_ptr = buffer;

      while (remaining > 0)
      {
         uint64_t chunk = (remaining < max_chunk) ? remaining : max_chunk;

         if (!copy_from_user(temp, current_user_ptr, chunk))
            return ERR_FAULT;

         for (uint64_t i = 0; i < chunk; ++i)
         {
            char text[2];
            text[0] = static_cast<char>(temp[i]);
            text[1] = '\0';
            uart_puts(text);
         }

         total_written += chunk;
         current_user_ptr += chunk;
         remaining -= chunk;
      }

      return static_cast<int64_t>(total_written);
   }
}

void syscall_handle(TrapFrame *frame)
{
   const uint64_t syscall_number = frame->a7;
   const uint64_t arg0 = frame->a0;
   const uint64_t arg1 = frame->a1;
   const uint64_t arg2 = frame->a2;

   switch (syscall_number)
   {
   case SYSCALL_WRITE:
      frame->a0 = static_cast<uint64_t>(
          syscall_write(arg0, arg1, arg2));
      break;

   case SYSCALL_EXIT:
      uart_puts("User task exited.\n");
      panic("User task exit.");

   default:
      frame->a0 = static_cast<uint64_t>(ERR_NOSYS);
      break;
   }

   frame->epc += 4;
}