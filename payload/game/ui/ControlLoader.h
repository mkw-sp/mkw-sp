#pragma once

#include "UIControl.h"

typedef struct {
    LayoutUIControl *control;
    u8 _04[0x28 - 0x04];
} ControlLoader;
static_assert(sizeof(ControlLoader) == 0x28);

void ControlLoader_load(ControlLoader *this, const char *dir, const char *file, const char *variant,
        const char *const *groups);