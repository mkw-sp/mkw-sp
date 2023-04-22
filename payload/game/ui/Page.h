#pragma once

#include <Common.h>

typedef struct Page {
    u8 _00[0x44 - 0x00];
} Page;

static_assert(sizeof(Page) == 0x44);
