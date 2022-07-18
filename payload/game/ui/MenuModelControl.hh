#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class BackGroundModelControl : public LayoutUIControl {
public:
    void setModel(u32 model);

private:
    u8 _174[0x180 - 0x174];
};
static_assert(sizeof(BackGroundModelControl) == 0x180);

} // namespace UI
