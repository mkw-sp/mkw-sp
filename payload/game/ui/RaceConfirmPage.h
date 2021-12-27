#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x6c4 - 0x000];
    bool hasConfirmed;
    u8 _6c5[0x6cc - 0x6c5];
} RaceConfirmPage;
