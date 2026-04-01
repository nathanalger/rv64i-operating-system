#pragma once

[[noreturn]] void _panic(const char* function, const char* msg);

#define panic(msg) _panic(__func__, msg)