#pragma once

#include "../Page.h"

typedef struct {
    Page;
    u8 _044[0x1dc - 0x044];
} RacePage;
