#include "UIControl.h"

#include "ControlLoader.h"

void LayoutUIControl_load(LayoutUIControl *this, const char *dir, const char *file,
        const char *variant, const char *const *groups) {
    ControlLoader loader;
    loader.control = this;
    ControlLoader_load(&loader, dir, file, variant, groups);
}
