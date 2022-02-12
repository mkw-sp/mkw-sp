#include "UIControl.h"

#include <revolution.h>
#include <stdio.h>
#include "ControlLoader.h"

void LayoutUIControl_load(LayoutUIControl *this, const char *dir, const char *file,
        const char *variant, const char *const *groups) {
    ControlLoader loader;
    loader.control = this;
    ControlLoader_load(&loader, dir, file, variant, groups);
}

void *ControlCheckValid(void *result, const char *path) {
    if (result)
        return result;

    char buf[128];
    snprintf(buf, sizeof(buf), "Failed to load %s\n", path);

    const GXColor fg = { 255, 255, 255, 255 };
    const GXColor bg = { 0, 0, 0, 255 };
    OSFatal(fg, bg, buf);

    __builtin_unreachable();
}
