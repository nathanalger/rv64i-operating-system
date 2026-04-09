#include "Syscall.hpp"
#include "Traps.hpp"
#include "UART.hpp"
#include "CSR.hpp"
#include "PrintHex.hpp"
#include <stdint.h>

namespace
{
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
      const volatile uint8_t *in =
          reinterpret_cast<const volatile uint8_t *>(user_src);

      for (uint64_t i = 0; i < size; ++i)
         out[i] = in[i];

      csr_write_sstatus(old_status);
      return true;
   }

   static bool copy_to_user(uint64_t user_dst, const void *src, uint64_t size)
   {
      if (src == nullptr)
         return false;

      if (user_dst == 0 && size != 0)
         return false;

      uint64_t old_status = csr_read_sstatus();
      csr_write_sstatus(old_status | SSTATUS_SUM);

      volatile uint8_t *out =
          reinterpret_cast<volatile uint8_t *>(user_dst);
      const uint8_t *in = static_cast<const uint8_t *>(src);

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

      uint8_t temp[256];
      uint64_t total_written = 0;

      while (length > 0)
      {
         uint64_t chunk = (length < sizeof(temp)) ? length : sizeof(temp);

         if (!copy_from_user(temp, buffer, chunk))
            return ERR_FAULT;

         for (uint64_t i = 0; i < chunk; ++i)
         {
            char text[2];
            text[0] = static_cast<char>(temp[i]);
            text[1] = '\0';
            uart_puts(text);
         }

         buffer += chunk;
         length -= chunk;
         total_written += chunk;
      }

      return static_cast<int64_t>(total_written);
   }

   static int64_t syscall_read(uint64_t fd, uint64_t buffer, uint64_t length)
   {
      if (fd != 0)
         return ERR_BADF;

      if (length == 0)
         return 0;

      uint8_t temp[256];
      uint64_t total_read = 0;

      if (length > sizeof(temp))
         length = sizeof(temp);

      while (total_read < length)
      {
         char c = uart_getc();

         // Normalize Enter
         if (c == '\r')
         {
            c = '\n';
         }

         // Handle backspace and delete
         if (c == '\b' || c == 0x7F)
         {
            if (total_read > 0)
            {
               total_read--;

               // Erase last character visually
               uart_putc('\b');
               uart_putc(' ');
               uart_putc('\b');
            }

            continue;
         }

         temp[total_read++] = static_cast<uint8_t>(c);

         // Echo typed character
         uart_putc(c);

         if (c == '\n')
            break;
      }

      if (!copy_to_user(buffer, temp, total_read))
         return ERR_FAULT;

      return static_cast<int64_t>(total_read);
   }

   [[noreturn]] static void syscall_exit(uint64_t status)
   {
      uart_puts("User task exited with status ");
      Utility::print_hex(status);
      uart_puts(".\n");

      while (1)
         asm volatile("wfi");
   }
}

void syscall_handle(TrapFrame *frame)
{
   const uint64_t syscall_number = frame->a7;

   switch (syscall_number)
   {
   case SYSCALL_READ:
      frame->a0 = static_cast<uint64_t>(
          syscall_read(frame->a0, frame->a1, frame->a2));
      frame->epc += 4;
      return;

   case SYSCALL_WRITE:
      frame->a0 = static_cast<uint64_t>(
          syscall_write(frame->a0, frame->a1, frame->a2));
      frame->epc += 4;
      return;

   case SYSCALL_EXIT:
      frame->epc += 4;
      syscall_exit(frame->a0);

   case SYSCALL_EXIT_GROUP:
      frame->epc += 4;
      syscall_exit(frame->a0);

   default:
      frame->a0 = static_cast<uint64_t>(ERR_NOSYS);
      frame->epc += 4;
      return;
   }
}