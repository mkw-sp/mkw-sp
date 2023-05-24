#pragma once

#include <Common.hh>

namespace UI {

class Page;
class UIControl;

class ControlGroup {
public:
    ~ControlGroup();
    void dt(s32 type);
    void calc();

    void logDebug(int depth = 0);

    UIControl **m_data;
    UIControl **m_dataSorted; // By z_index
    UIControl *m_parent;
    Page *m_page;
    s32 m_size;
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
