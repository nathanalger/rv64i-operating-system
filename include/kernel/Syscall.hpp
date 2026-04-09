#pragma once
#include <stdint.h>

struct TrapFrame;

void syscall_handle(TrapFrame *frame);