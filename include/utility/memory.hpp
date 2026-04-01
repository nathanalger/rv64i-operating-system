#pragma once

#include <stddef.h>

extern "C"
{
   void* memcpy(void* dest, const void* src, size_t n);
   void* memset(void* dest, int c, size_t n);
   int   memcmp(const void* a, const void* b, size_t n);
   void* memmove(void* dest, const void* src, size_t n);
   size_t strlen(const char* s);
   void* memchr(const void* ptr, int c, size_t n);
   char* strchr(const char* s, int c);
   char* strrchr(const char* s, int c);
   size_t strnlen(const char* s, size_t maxlen);
   unsigned long strtoul(const char* nptr, char** endptr, int base);
}
