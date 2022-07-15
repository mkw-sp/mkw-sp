#pragma once

#include "game/ui/Page.hh"

namespace UI {

class MenuPage : public Page {
public:
    static TypeInfo *GetTypeInfo();

private:
    u8 _044[0x3ec - 0x044];

public:
    PageId m_prevId;

private:
    u8 _3f0[0x430 - 0x3f0];
};
static_assert(sizeof(MenuPage) == 0x430);

} // namespace UI
