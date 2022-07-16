#pragma once

__attribute__((noreturn, format(printf, 1, 2))) void panic(const char* format, ...);
