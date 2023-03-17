#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class ControlLoader {
public:
    ControlLoader(LayoutUIControl *control);
    ~ControlLoader();

    // Note: If you are using this to reimplement a Page you should be
    // using LayoutUIControl::load, which wraps creation and calling this.
    void load(const char *dir, const char *file, const char *variant, const char *const *groups);

private:
    LayoutUIControl *m_control;

    u8 _04[0x28 - 0x04];
};

static_assert(sizeof(ControlLoader) == 0x28);

} // namespace UI
