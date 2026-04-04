#pragma once
#include <stdint.h>

struct TrapFrame
{
   uint64_t ra;
   uint64_t sp;
   uint64_t gp;
   uint64_t tp;
   uint64_t t0;
   uint64_t t1;
   uint64_t t2;
   uint64_t s0;
   uint64_t s1;
   uint64_t a0;
   uint64_t a1;
   uint64_t a2;
   uint64_t a3;
   uint64_t a4;
   uint64_t a5;
   uint64_t a6;
   uint64_t a7;
   uint64_t s2;
   uint64_t s3;
   uint64_t s4;
   uint64_t s5;
   uint64_t s6;
   uint64_t s7;
   uint64_t s8;
   uint64_t s9;
   uint64_t s10;
   uint64_t s11;
   uint64_t t3;
   uint64_t t4;
   uint64_t t5;
   uint64_t t6;

   uint64_t epc;
   uint64_t status;
   uint64_t cause;
   uint64_t tval;
};

extern "C" void supervisor_trap_entry();
extern "C" void machine_trap_entry();

extern "C" void supervisor_trap_handler(TrapFrame *frame);
extern "C" void machine_trap_handler(TrapFrame *frame);

void machine_traps_init();
void supervisor_traps_init();