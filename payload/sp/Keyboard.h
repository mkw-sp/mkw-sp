#pragma once

#include <Common.h>

void SP_InitConsoleInput(void);
bool SP_IsConsoleInputInit(void);
void SP_DestroyConsoleInput(void);
void SP_ProcessConsoleInput(void);

typedef void (*OnLineCallback)(const char *buf, size_t len);

void SP_SetLineCallback(OnLineCallback callback);