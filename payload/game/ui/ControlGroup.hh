#pragma once

#include <Common.hh>

namespace UI {

class UIControl;

class ControlGroup {
public:
    ~ControlGroup();

private:
    UIControl *m_controls;
    UIControl *m_sortedControls;
    u8 _08[0x14 - 0x08];
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
