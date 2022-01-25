#pragma once

#include "Page.h"

typedef struct {
    Page;
    u8 _044[0x6c4 - 0x044];
    bool hasConfirmed;
    u8 _6c5[0x6cc - 0x6c5];
} RaceConfirmPage;
static_assert(sizeof(RaceConfirmPage) == 0x6cc);
