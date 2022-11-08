#pragma once

#include <Common.h>

// Other values are characters
typedef enum SimpleEvent {
    kSimpleEvent_Enter = 0xff,
    kSimpleEvent_Backspace = 0xfe,
    kSimpleEvent_Escape = 0xfd,
    kSimpleEvent_Tab = 0xfc,
    kSimpleEvent_ArrowL = 0xfb,
    kSimpleEvent_ArrowR = 0xfa,
    kSimpleEvent_ArrowD = 0xf0,
    kSimpleEvent_ArrowU = 0xef,
} SimpleEvent;

enum {
    kSimpleMods_CTRL = (1),
};

// Return if the console is available
bool SP_InitConsoleInput(void);
bool SP_IsConsoleInputInit(void);
void SP_DestroyConsoleInput(void);
void SP_ProcessConsoleInput(void);

typedef void (*SP_LineCallback)(const char *buf, size_t len);
typedef void (*SP_KeypressCallback)(char key, u8 mods, void *userdata);

// Invoked when ENTER is pressed
void SP_SetLineCallback(SP_LineCallback callback);

// When the console is inactive, keypresses will be sent here
void SP_SetKeypressCallback(SP_KeypressCallback callback, void *userdata);

typedef struct SP_Line {
    const char *buf;
    size_t len;
} SP_Line;

// The scratch buffer before ENTER is pressed.
SP_Line SP_GetCurrentLine(void);

// Is the console open?
bool SP_IsTyping(void);
