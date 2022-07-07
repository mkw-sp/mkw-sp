#pragma once

#include <nw4r/lyt/lyt_pane.hh>

namespace UI {

class Layout {
public:
    nw4r::lyt::Pane *findPaneByName(const char *name);

private:
    u8 _00[0x28 - 0x00];
};
static_assert(sizeof(Layout) == 0x28);

class MainLayout : public Layout {
public:
    void dt(s32 type);

private:
    u8 _28[0x9c - 0x28];
};
static_assert(sizeof(MainLayout) == 0x9c);

} // namespace UI
