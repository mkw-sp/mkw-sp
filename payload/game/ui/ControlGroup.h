#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x08 - 0x00];
    void* parent;
    struct Page *page;
    s32 capacity;
} ControlGroup;
static_assert(sizeof(ControlGroup) == 0x14);

void ControlGroup_calc(ControlGroup* group);
