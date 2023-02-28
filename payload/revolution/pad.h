#pragma once

#include <Common.h>

enum {
    PAD_BUTTON_LEFT = 0x0001,
    PAD_BUTTON_RIGHT = 0x0002,
    PAD_BUTTON_DOWN = 0x0004,
    PAD_BUTTON_UP = 0x0008,
    PAD_TRIGGER_Z = 0x0010,
    PAD_TRIGGER_R = 0x0020,
    PAD_TRIGGER_L = 0x0040,
    PAD_BUTTON_A = 0x0100,
    PAD_BUTTON_B = 0x0200,
    PAD_BUTTON_X = 0x0400,
    PAD_BUTTON_Y = 0x0800,
    PAD_BUTTON_START = 0x1000,
};

typedef struct PADStatus {
    u16 buttons;
    s8 stickX;
    s8 stickY;
    u8 _04[0xc - 0x4];
} PADStatus;
static_assert(sizeof(PADStatus) == 0xc);

void PADRead(PADStatus *);
void PADClampCircle(PADStatus *);
