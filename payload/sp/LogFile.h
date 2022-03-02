#pragma once

#include <revolution.h>

#include <stdarg.h>

void LogFile_init(void);

void LogFile_vprintf(const char *restrict format, va_list args);

typedef struct ThreadNode ThreadNode;

struct ThreadNode {
    OSThread *thread;
    ThreadNode *next;
};

typedef struct {
    ThreadNode node;
} LogFileDisableGuard;

void LogFile_disable(LogFileDisableGuard *guard);

void LogFile_restore(LogFileDisableGuard *guard);

#define LOG_FILE_DISABLE() \
    LogFileDisableGuard _guard __attribute__((cleanup(LogFile_restore))); \
    LogFile_disable(&_guard);
