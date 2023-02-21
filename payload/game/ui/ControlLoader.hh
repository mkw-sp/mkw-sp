#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class ControlLoader: public LayoutUIControl {
public:
    ControlLoader(LayoutUIControl* control);
    ~ControlLoader() override;

    void load(const char *dir, const char *file, const char *variant, const char *const *groups);
};

}
