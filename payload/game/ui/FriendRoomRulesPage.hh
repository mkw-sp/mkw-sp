#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"

#include <sp/settings/RoomSettings.hh>

namespace UI {

class FriendRoomRulesPage : public Page {
public:
    FriendRoomRulesPage();
    FriendRoomRulesPage(const FriendRoomRulesPage &) = delete;
    FriendRoomRulesPage(FriendRoomRulesPage &&) = delete;
    ~FriendRoomRulesPage() override;

    void onInit() override;
    void beforeInAnim() override;
    void afterOutAnim() override;
    void beforeCalc() override;
    void afterCalc() override;

    void pop(Anim anim);
    void refresh(const std::array<u32, SP::RoomSettings::count> &settings);

private:
    void onFront(u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<FriendRoomRulesPage>;

    PageInputManager m_inputManager;
    LayoutUIControlScaleFade m_rules;
    MessageWindowControlScaleFade m_messageWindow;
    LayoutUIControlScaleFade m_okKey;
    BlackBackControl m_blackBack;
    H<PageInputManager> m_onFront{ this, &FriendRoomRulesPage::onFront };
    bool m_popRequested;
};

} // namespace UI
