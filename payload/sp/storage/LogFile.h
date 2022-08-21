#pragma once

#include <stdarg.h>

#define LOG_FILE_DIRECTORY L"/mkw-sp/logs"
#define LOG_FILE_EXTENSION L".log"
#define LOG_FILE_NAME_LENGTH 23
#define LOG_FILE_EXTENSION_LENGTH (sizeof(LOG_FILE_EXTENSION) / sizeof(wchar_t) - 1)

void LogFile_Init(void);

void LogFile_VPrintf(const char *format, va_list args);
