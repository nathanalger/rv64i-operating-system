#pragma once
#include <stdint.h>

constexpr uint64_t SSTATUS_SIE  = 1ULL << 1;
constexpr uint64_t SSTATUS_SPIE = 1ULL << 5;
constexpr uint64_t SSTATUS_SPP  = 1ULL << 8;

constexpr uint64_t SATP_MODE_BARE = 0ULL;
constexpr uint64_t SATP_MODE_SV39 = 8ULL;

namespace Utility {
   static inline uint64_t csr_read_sstatus()
   {
      uint64_t value;
      asm volatile("csrr %0, sstatus" : "=r"(value));
      return value;
   }

   static inline void csr_write_sstatus(uint64_t value)
   {
      asm volatile("csrw sstatus, %0" :: "r"(value));
   }

   static inline uint64_t csr_read_satp()
   {
      uint64_t value;
      asm volatile("csrr %0, satp" : "=r"(value));
      return value;
   }

   static inline void csr_write_satp(uint64_t value)
   {
      asm volatile("csrw satp, %0" :: "r"(value));
   }

   static inline uint64_t csr_read_stvec()
   {
      uint64_t value;
      asm volatile("csrr %0, stvec" : "=r"(value));
      return value;
   }

   static inline void csr_write_stvec(uint64_t value)
   {
      asm volatile("csrw stvec, %0" :: "r"(value));
   }

   static inline uint64_t csr_read_sepc()
   {
      uint64_t value;
      asm volatile("csrr %0, sepc" : "=r"(value));
      return value;
   }

   static inline void csr_write_sepc(uint64_t value)
   {
      asm volatile("csrw sepc, %0" :: "r"(value));
   }

   static inline uint64_t csr_read_scause()
   {
      uint64_t value;
      asm volatile("csrr %0, scause" : "=r"(value));
      return value;
   }

   static inline uint64_t csr_read_sie()
   {
      uint64_t value;
      asm volatile("csrr %0, sie" : "=r"(value));
      return value;
   }

   static inline void csr_write_sie(uint64_t value)
   {
      asm volatile("csrw sie, %0" :: "r"(value));
   }

   static inline void csr_set_bits_sstatus(uint64_t bits)
   {
      asm volatile("csrs sstatus, %0" :: "r"(bits));
   }

   static inline void csr_clear_bits_sstatus(uint64_t bits)
   {
      asm volatile("csrc sstatus, %0" :: "r"(bits));
   }

   static inline void csr_set_bits_sie(uint64_t bits)
   {
      asm volatile("csrs sie, %0" :: "r"(bits));
   }

   static inline void csr_clear_bits_sie(uint64_t bits)
   {
      asm volatile("csrc sie, %0" :: "r"(bits));
   }

   static inline void sfence_vma()
   {
      asm volatile("sfence.vma zero, zero" ::: "memory");
   }

   static inline uint64_t make_satp_sv39(uint64_t root_table_physical_address)
   {
      return (SATP_MODE_SV39 << 60) | (root_table_physical_address >> 12);
   }
}