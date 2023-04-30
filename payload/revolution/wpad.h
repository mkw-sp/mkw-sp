#pragma once

#include <Common.h>

enum {
    WPAD_BUTTON_LEFT = 0x1,
    WPAD_BUTTON_RIGHT = 0x2,
    WPAD_BUTTON_DOWN = 0x4,
    WPAD_BUTTON_UP = 0x8,
    WPAD_BUTTON_HOME = 0x8000,
};

enum {
    WPAD_CL_BUTTON_UP = 0x0001,
    WPAD_CL_BUTTON_LEFT = 0x0002,
    WPAD_CL_TRIGGER_ZR = 0x0004,
    WPAD_CL_BUTTON_X = 0x0008,
    WPAD_CL_BUTTON_A = 0x0010,
    WPAD_CL_BUTTON_Y = 0x0020,
    WPAD_CL_BUTTON_B = 0x0040,
    WPAD_CL_TRIGGER_ZL = 0x0080,
    WPAD_CL_RESERVED = 0x0100,
    WPAD_CL_TRIGGER_R = 0x0200,
    WPAD_CL_BUTTON_PLUS = 0x0400,
    WPAD_CL_BUTTON_HOME = 0x0800,
    WPAD_CL_BUTTON_MINUS = 0x1000,
    WPAD_CL_TRIGGER_L = 0x2000,
    WPAD_CL_BUTTON_DOWN = 0x4000,
    WPAD_CL_BUTTON_RIGHT = 0x8000,
};

typedef struct WPADCLStatus {
    u8 _00[0x2a - 0x00];
    u16 buttons;
    s16 lStickX;
    s16 lStickY;
    u8 _30[0x38 - 0x30];
} WPADCLStatus;
static_assert(sizeof(WPADCLStatus) == 0x38);

s32 WPADProbe(s32, u32 *);
