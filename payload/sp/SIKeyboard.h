#pragma once

#include <Common.h>

//! 1. Search for a GCKeyboard device in slots 0-3
//! 2. If found, try to connect to it
//! 3. If connected, enable the background service to grab keypresses
void SIKeyboard_InitSimple(void);

//! Search for a channel index with a GCKeyboard inserted. Return -1 if none.
s32 SIKeyboard_Scan(void);

void SIKeyboard_Connect(u32 channel);
void SIKeyboard_Disconnect(void);

s32 SIKeyboard_GetCurrentConnection(void);

// Polling
size_t SIKeyboard_Poll(char *keys, size_t max_keys);

// Interrupts
//
// Call once, and keyboard events will be buffered in the background.
bool SIKeyboard_EnableBackgroundService(void);
size_t SIKeyboard_ConsumeBuffer(char *buf, size_t max_take);

// Non-printable characters
enum {
    SIKEY_HOME = 6,
    SIKEY_END = 7,
    SIKEY_PGUP = 8,
    SIKEY_PGDN = 9,
    SIKEY_SCROLL_LOCK = 0xA,

    SIKEY_PRINTSCR = 0x36,

    SIKEY_F1 = 0x40,
    SIKEY_F2 = 0x41,
    SIKEY_F3 = 0x42,
    SIKEY_F4 = 0x43,
    SIKEY_F5 = 0x44,
    SIKEY_F6 = 0x45,
    SIKEY_F7 = 0x46,
    SIKEY_F8 = 0x47,
    SIKEY_F9 = 0x48,
    SIKEY_F10 = 0x49,
    SIKEY_F11 = 0x4A,
    SIKEY_F12 = 0x4B,

    SIKEY_ESC = 0x4C,
    SIKEY_INSERT = 0x4D,
    SIKEY_DELETE = 0x4E,

    SIKEY_BACKSPACE = 0x50,
    SIKEY_TAB = 0x51,
    SIKEY_CAPSLOCK = 0x53,

    //
    // These are not exposed by the driver. Instead, query SIKEY_NEXT_IS_SHIFTED.
    //
    SIKEY_LEFTSHIFT = 0x54,
    SIKEY_RIGHTSHIFT = 0x55,
    //

    SIKEY_LEFTCONTROL = 0x56,
    SIKEY_RIGHTALT = 0x57,
    SIKEY_LEFTWINDOWS = 0x58,

    SIKEY_RIGHTWINDOWS = 0x5A,
    SIKEY_MENU = 0x5B,
    SIKEY_LEFTARROW = 0x5C,
    SIKEY_DOWNARROW = 0x5D,
    SIKEY_UPARROW = 0x5E,
    SIKEY_RIGHTARROW = 0x5F,
    SIKEY_ENTER = 0x61,

    SIKEY_NEXT_IS_SHIFTED = 0xFF,
};

bool SIKeyboard_KeycodeIsCharacter(char key);
char SIKeyboard_KeycodeToCharacter(char key, bool shift);
