#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x0c - 0x00];
    struct Page *page;
    u8 _10[0x14 - 0x10];
} ControlGroup;
static_assert(sizeof(ControlGroup) == 0x14);
