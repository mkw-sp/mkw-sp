#include "UIControl.h"

#include "ControlLoader.h"
#include <revolution.h>
#include <stdio.h>

void LayoutUIControl_load(LayoutUIControl *this, const char *dir, const char *file,
        const char *variant, const char *const *groups) {
    ControlLoader loader;
    loader.control = this;
    ControlLoader_load(&loader, dir, file, variant, groups);
}

// TODO: Hack
void _ZN2UI15LayoutUIControl4loadEPKcS2_S2_PKS2_(LayoutUIControl *this, const char *dir,
        const char *file, const char *variant, const char *const *groups) {
    LayoutUIControl_load(this, dir, file, variant, groups);
}

void *ControlCheckValid(void *result, const char *path) {
    if (result) {
#if LOG_CONTROL_LOAD
        SP_LOG("Control loaded: %s", path);
#endif
        return result;
    }

    panic("Failed to load '%s'!", path);
}