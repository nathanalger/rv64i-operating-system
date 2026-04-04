#pragma once
#include <stdint.h>

constexpr uint64_t SSTATUS_SIE = 1ULL << 1;
constexpr uint64_t SSTATUS_SPIE = 1ULL << 5;
constexpr uint64_t SSTATUS_SPP = 1ULL << 8;

constexpr uint64_t SATP_MODE_BARE = 0ULL;
constexpr uint64_t SATP_MODE_SV39 = 8ULL;

constexpr uint64_t MSTATUS_MIE = 1ULL << 3;
constexpr uint64_t MSTATUS_MPIE = 1ULL << 7;
constexpr uint64_t MSTATUS_MPP_MASK = 3ULL << 11;
constexpr uint64_t MSTATUS_MPP_U = 0ULL << 11;
constexpr uint64_t MSTATUS_MPP_S = 1ULL << 11;
constexpr uint64_t MSTATUS_MPP_M = 3ULL << 11;

extern "C" void supervisor_main();
extern "C" void enter_supervisor_mode();

static inline uint64_t csr_read_sstatus()
{
   uint64_t value;
   asm volatile("csrr %0, sstatus" : "=r"(value));
   return value;
}

static inline void csr_write_sstatus(uint64_t value)
{
   asm volatile("csrw sstatus, %0" ::"r"(value));
}

static inline uint64_t csr_read_satp()
{
   uint64_t value;
   asm volatile("csrr %0, satp" : "=r"(value));
   return value;
}

static inline void csr_write_satp(uint64_t value)
{
   asm volatile("csrw satp, %0" ::"r"(value));
}

static inline uint64_t csr_read_stvec()
{
   uint64_t value;
   asm volatile("csrr %0, stvec" : "=r"(value));
   return value;
}

static inline void csr_write_stvec(uint64_t value)
{
   asm volatile("csrw stvec, %0" ::"r"(value));
}

static inline uint64_t csr_read_sepc()
{
   uint64_t value;
   asm volatile("csrr %0, sepc" : "=r"(value));
   return value;
}

static inline void csr_write_sepc(uint64_t value)
{
   asm volatile("csrw sepc, %0" ::"r"(value));
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
   asm volatile("csrw sie, %0" ::"r"(value));
}

static inline void csr_set_bits_sstatus(uint64_t bits)
{
   asm volatile("csrs sstatus, %0" ::"r"(bits));
}

static inline void csr_clear_bits_sstatus(uint64_t bits)
{
   asm volatile("csrc sstatus, %0" ::"r"(bits));
}

static inline void csr_set_bits_sie(uint64_t bits)
{
   asm volatile("csrs sie, %0" ::"r"(bits));
}

static inline void csr_clear_bits_sie(uint64_t bits)
{
   asm volatile("csrc sie, %0" ::"r"(bits));
}

static inline uint64_t csr_read_mstatus()
{
   uint64_t value;
   asm volatile("csrr %0, mstatus" : "=r"(value));
   return value;
}

static inline void csr_write_mstatus(uint64_t value)
{
   asm volatile("csrw mstatus, %0" ::"r"(value));
}

static inline uint64_t csr_read_mtvec()
{
   uint64_t value;
   asm volatile("csrr %0, mtvec" : "=r"(value));
   return value;
}

static inline void csr_write_mtvec(uint64_t value)
{
   asm volatile("csrw mtvec, %0" ::"r"(value));
}

static inline uint64_t csr_read_mepc()
{
   uint64_t value;
   asm volatile("csrr %0, mepc" : "=r"(value));
   return value;
}

static inline void csr_write_mepc(uint64_t value)
{
   asm volatile("csrw mepc, %0" ::"r"(value));
}

static inline uint64_t csr_read_mcause()
{
   uint64_t value;
   asm volatile("csrr %0, mcause" : "=r"(value));
   return value;
}

static inline uint64_t csr_read_mie()
{
   uint64_t value;
   asm volatile("csrr %0, mie" : "=r"(value));
   return value;
}

static inline void csr_write_mie(uint64_t value)
{
   asm volatile("csrw mie, %0" ::"r"(value));
}

static inline uint64_t csr_read_medeleg()
{
   uint64_t value;
   asm volatile("csrr %0, medeleg" : "=r"(value));
   return value;
}

static inline void csr_write_medeleg(uint64_t value)
{
   asm volatile("csrw medeleg, %0" ::"r"(value));
}

static inline uint64_t csr_read_mideleg()
{
   uint64_t value;
   asm volatile("csrr %0, mideleg" : "=r"(value));
   return value;
}

static inline void csr_write_mideleg(uint64_t value)
{
   asm volatile("csrw mideleg, %0" ::"r"(value));
}

static inline void csr_set_bits_mstatus(uint64_t bits)
{
   asm volatile("csrs mstatus, %0" ::"r"(bits));
}

static inline void csr_clear_bits_mstatus(uint64_t bits)
{
   asm volatile("csrc mstatus, %0" ::"r"(bits));
}

static inline void csr_set_bits_mie(uint64_t bits)
{
   asm volatile("csrs mie, %0" ::"r"(bits));
}

static inline void csr_clear_bits_mie(uint64_t bits)
{
   asm volatile("csrc mie, %0" ::"r"(bits));
}

static inline uint64_t csr_read_mtval()
{
   uint64_t value;
   asm volatile("csrr %0, mtval" : "=r"(value));
   return value;
}

static inline void sfence_vma()
{
   asm volatile("sfence.vma zero, zero" ::: "memory");
}

static inline uint64_t make_satp_sv39(uint64_t root_table_physical_address)
{
   return (SATP_MODE_SV39 << 60) | (root_table_physical_address >> 12);
}

static inline void csr_write_pmpaddr0(uint64_t value)
{
   asm volatile("csrw pmpaddr0, %0" ::"r"(value));
}

static inline void csr_write_pmpcfg0(uint64_t value)
{
   asm volatile("csrw pmpcfg0, %0" ::"r"(value));
}

static inline void csr_set_mstatus_mpp_supervisor()
{
   uint64_t value = csr_read_mstatus();
   value &= ~MSTATUS_MPP_MASK;
   value |= MSTATUS_MPP_S;
   csr_write_mstatus(value);
}

static inline void csr_set_mstatus_mpp_machine()
{
   uint64_t value = csr_read_mstatus();
   value &= ~MSTATUS_MPP_MASK;
   value |= MSTATUS_MPP_M;
   csr_write_mstatus(value);
}

void delegate_supervisor_exceptions();
void pmp_init_allow_all();