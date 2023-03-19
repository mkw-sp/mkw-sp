#pragma once

#include "game/ui/Button.hh"
#include "game/ui/NumericEditBox.hh"
#include "game/ui/Page.hh"

namespace UI {

class OnlineFriendRegisterPage : public Page {
public:
    PageId replacedGetReplacement();
    REPLACE void onOkButtonFront(PushButton *button, u32 localPlayerId);

private:
    u8 _0044[0x1c34 - 0x0044];
    NumericEditBox m_editBox;
    u8 _1ea8[0x2530 - 0x1ea8];
    PageId m_replacement;
};

static_assert(sizeof(OnlineFriendRegisterPage) == 0x2534);

} // namespace UI
