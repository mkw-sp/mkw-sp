#pragma once

#include "../Page.h"

typedef struct {
    Page;
    u8 _044[0x344 - 0x044];
} RaceMenuPage;
static_assert(sizeof(RaceMenuPage) == 0x344);
