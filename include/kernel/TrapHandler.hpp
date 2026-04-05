#include <cstdint>
#include "Traps.hpp"

void trap_handler(TrapFrame *frame);
void handle_exception(TrapFrame *frame);
void handle_interrupt(TrapFrame *frame);
void handle_supervisor_exception(TrapFrame *frame);
void handle_user_exception(TrapFrame *frame);
void advance_trap(TrapFrame *frame);