#include "utility/memory.hpp"

extern "C"
{

   void* memcpy(void* dest, const void* src, size_t n)
   {
      auto* d = (unsigned char*)dest;
      auto* s = (const unsigned char*)src;

      for (size_t i = 0; i < n; ++i)
         d[i] = s[i];

      return dest;
   }

   void* memset(void* dest, int c, size_t n)
   {
      auto* d = (unsigned char*)dest;

      for (size_t i = 0; i < n; ++i)
         d[i] = (unsigned char)c;

      return dest;
   }

   int memcmp(const void* a, const void* b, size_t n)
   {
      auto* x = (const unsigned char*)a;
      auto* y = (const unsigned char*)b;

      for (size_t i = 0; i < n; ++i)
      {
         if (x[i] != y[i])
               return x[i] - y[i];
      }

      return 0;
   }

   void* memmove(void* dest, const void* src, size_t n)
   {
      auto* d = (unsigned char*)dest;
      auto* s = (const unsigned char*)src;

      if (d < s)
      {
         for (size_t i = 0; i < n; ++i)
               d[i] = s[i];
      }
      else
      {
         for (size_t i = n; i != 0; --i)
               d[i - 1] = s[i - 1];
      }

      return dest;
   }

   size_t strlen(const char* s)
   {
      size_t len = 0;

      while (s[len])
         ++len;

      return len;
   }

   void* memchr(const void* ptr, int c, size_t n)
   {
      const unsigned char* p = (const unsigned char*)ptr;

      for (size_t i = 0; i < n; ++i)
      {
         if (p[i] == (unsigned char)c) return (void*)(p + i);
      }

      return nullptr;
   }

   char* strchr(const char* s, int c)
   {
      while (*s)
      {
         if (*s == (char)c)
               return (char*)s;
         ++s;
      }

      return (c == 0) ? (char*)s : nullptr;
   }

   char* strrchr(const char* s, int c)
   {
      const char* last = nullptr;

      while (*s)
      {
         if (*s == (char)c)
               last = s;
         ++s;
      }

      if (c == 0)
         return (char*)s;

      return (char*)last;
   }

   size_t strnlen(const char* s, size_t maxlen)
   {
      size_t len = 0;

      while (len < maxlen && s[len])
         ++len;

      return len;
   }

   unsigned long strtoul(const char* nptr, char** endptr, int base)
   {
      unsigned long result = 0;

      while (*nptr == ' ' || *nptr == '\t')
         ++nptr;

      if (base == 0)
      {
         if (nptr[0] == '0' && (nptr[1] == 'x' || nptr[1] == 'X'))
         {
               base = 16;
               nptr += 2;
         }
         else
         {
               base = 10;
         }
      }

      while (*nptr)
      {
         char c = *nptr;
         int digit = -1;

         if (c >= '0' && c <= '9')
               digit = c - '0';
         else if (c >= 'a' && c <= 'f')
               digit = 10 + (c - 'a');
         else if (c >= 'A' && c <= 'F')
               digit = 10 + (c - 'A');
         else
               break;

         if (digit >= base)
               break;

         result = result * base + digit;
         ++nptr;
      }

      if (endptr)
         *endptr = (char*)nptr;

      return result;
   }
}