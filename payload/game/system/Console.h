#pragma once

#include <stdarg.h>

void Console_init(void);
void Console_draw(void);
void Console_calc(void);
void Console_addLine(const char *s, size_t len);
void Console_vprintf(const char* prefix, const char *s, va_list args);
