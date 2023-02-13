#pragma once

#include "game/ui/Page.hh"

#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineModeSelectPage : public Page {
private:
    REPLACE void onActivate() override;

    u8 _44[0x94 - 0x44];
    CtrlMenuPageTitleText m_pageTitleText;
    u8 _208[0xcb0 - 0x208];
};

static_assert(sizeof(OnlineModeSelectPage) == 0xcb0);

}
