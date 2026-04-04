#include "CSR.hpp"

void delegate_supervisor_exceptions()
{
   uint64_t deleg = 0;
   deleg |= (1ULL << 1);
   deleg |= (1ULL << 3);
   deleg |= (1ULL << 8);
   deleg |= (1ULL << 9);
   deleg |= (1ULL << 12);
   deleg |= (1ULL << 13);
   deleg |= (1ULL << 15);
   csr_write_medeleg(deleg);
}

void pmp_init_allow_all()
{
   const uint64_t pmpcfg = 0x0F;
   csr_write_pmpaddr0(0x3FFFFFFFFFFFFFFFULL);
   csr_write_pmpcfg0(pmpcfg);
}

extern "C" void enter_supervisor_mode()
{
   csr_write_mepc((uint64_t)supervisor_main);
   csr_set_mstatus_mpp_supervisor();
   asm volatile("mret");
}