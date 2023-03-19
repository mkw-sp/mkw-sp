#pragma once

#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBusySymbol.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class AwaitPage : public Page {
public:
    AwaitPage();
    ~AwaitPage() override;

    void onInit() override;
    void onActivate() override;
    void afterCalc() override;

    virtual void reset() = 0;
    virtual void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) = 0;

    void pop();

private:
    MenuInputManager m_inputManager;
    BOOL m_isPopped;
};

class MenuAwaitPage : public AwaitPage {
public:
    MenuAwaitPage();
    ~MenuAwaitPage() override;

    PageId getReplacement() override;
    void onInit() override;

    void reset() override;
    void setWindowMessage(u32 messageId, MessageInfo *info = nullptr) override;

    void setTitleMessage(u32 messageId, MessageInfo *info = nullptr);
    void setReplacement(PageId replacement);

private:
    CtrlMenuPageTitleText m_titleText;
    CtrlMenuBusySymbol m_busySymbol;
    MessageWindowControl *m_messageWindow;
    PageId m_replacement;
};

class SPAwaitPage : public MenuAwaitPage {
public:
    SPAwaitPage();
    ~SPAwaitPage() override;

    void onActivate() override;
};

static_assert(sizeof(AwaitPage) == 0x58);
static_assert(sizeof(MenuAwaitPage) == 0x348);
static_assert(sizeof(SPAwaitPage) == 0x348);

} // namespace UI
