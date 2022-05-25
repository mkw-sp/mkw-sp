#pragma once

#include <Common.hh>
#include <memory>

namespace UI {

class UIControl;
class Page;

class ControlGroup {
public:
    ~ControlGroup();

    std::unique_ptr<UIControl[]> m_controls;
    std::unique_ptr<UIControl[]> m_sortedControls;
    u8 _08[0xc - 0x08];
    Page *m_page;
    u8 _10[0x14 - 0x10];
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
