#pragma once
#include <stdint.h>

constexpr uint64_t SYSCALL_READ = 63;
constexpr uint64_t SYSCALL_WRITE = 64;
constexpr uint64_t SYSCALL_EXIT = 93;
constexpr uint64_t SYSCALL_EXIT_GROUP = 94;

struct TrapFrame;
void syscall_handle(TrapFrame *frame);