#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x6fa - 0x000];
    bool isLocal;
    u8 _6fb[0x6fc - 0x6fb];
    bool isGhost;
    u8 _6fd[0x708 - 0x6fd];
} DriverSound;
static_assert(sizeof(DriverSound) == 0x708);
