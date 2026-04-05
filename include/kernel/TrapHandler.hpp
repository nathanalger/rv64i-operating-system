#include <cstdint>
#include "Traps.hpp"

void panic_unexpected_user_trap();
void trap_handler(uint64_t code, TrapFrame *frame);
void handle_exception(uint64_t code, TrapFrame *frame);
void handle_interrupt(uint64_t code, TrapFrame *frame);
void advance_trap(TrapFrame *frame);