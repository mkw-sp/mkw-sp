#pragma once

#include <Common.hh>
#include <memory>

namespace UI {

class UIControl;

class ControlGroup {
public:
    ~ControlGroup();

private:
    std::unique_ptr<UIControl[]> m_controls;
    std::unique_ptr<UIControl[]> m_sortedControls;
    u8 _08[0x14 - 0x08];
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
