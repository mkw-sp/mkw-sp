#pragma once

#include <Common.h>

// Return if the console is available
bool SP_InitConsoleInput(void);
bool SP_IsConsoleInputInit(void);
void SP_DestroyConsoleInput(void);
void SP_ProcessConsoleInput(void);

typedef void (*SP_LineCallback)(const char *buf, size_t len);

// Invoked when ENTER is pressed
void SP_SetLineCallback(SP_LineCallback callback);

typedef struct SP_Line {
    const char *buf;
    size_t len;
} SP_Line;

// The scratch buffer before ENTER is pressed.
SP_Line SP_GetCurrentLine(void);
