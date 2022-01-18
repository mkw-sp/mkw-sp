#pragma once

#include "../Page.h"

typedef struct {
    Page;
    u8 _044[0x064 - 0x044];
    u32 watchedPlayerId;
    u8 _068[0x1dc - 0x068];
} RacePage;
