#pragma once

#include <Common.hh>

namespace UI {

class Page;

class ControlGroup {
public:
    ~ControlGroup();
    void dt(s32 type);
    void calc();

    u8 _00[0x0c - 0x00];
    Page *m_page;
    u8 _10[0x14 - 0x10];
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
