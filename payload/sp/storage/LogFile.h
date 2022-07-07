#pragma once

#include <stdarg.h>

void LogFile_Init(void);

void LogFile_VPrintf(const char *format, va_list args);

void LogFile_Flush(void);
