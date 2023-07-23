#pragma once

#include "game/ui/Button.hh"
#include "game/ui/GlobalContext.hh"
#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"

namespace UI {

class WifiDisconnectedPage : public Page {
public:
    WifiDisconnectedPage();

    void onInit() override;
    REPLACE void onActivate() override;
    void beforeInAnim() override;
    void afterCalc() override;

private:
    PageInputManager m_inputManager;
    // Unused with onActivate replacement
    GlobalContext::OnlineDisconnectInfo m_disconnectInfo;
    MessageWindowControl m_messageBox;
    LayoutUIControl m_okayButton;
    u8 m_onButtonFront[0x48c - 0x478];
};

static_assert(sizeof(WifiDisconnectedPage) == 0x48c);

} // namespace UI
