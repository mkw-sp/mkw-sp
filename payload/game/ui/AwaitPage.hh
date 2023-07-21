#pragma once

#include "game/ui/Page.hh"

#include "game/ui/ctrl/CtrlBusySymbol.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class AwaitPage : public Page {
public:
    virtual void reset() = 0;
    virtual void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) = 0;

    void pop();

private:
    u8 _44[0x55 - 0x44];
};

class MenuAwaitPage : public AwaitPage {
public:
    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);
    void setSpinnerVisible(bool visible);

private:
    CtrlMenuPageTitleText m_titleText;
    BusySymbol m_busySymbol;
    LayoutUIControl *m_messageWindow;
    PageId m_replacement;
};
static_assert(sizeof(MenuAwaitPage) == 0x348);

} // namespace UI
