#pragma once

#include "game/ui/MatchingMessageWindow.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/ctrl/CtrlMenuDummyBack.hh"

#include <sp/cs/RoomManager.hh>

namespace UI {

class FriendMatchingPage : public Page {
public:
    FriendMatchingPage();
    FriendMatchingPage(const FriendMatchingPage &) = delete;
    FriendMatchingPage(FriendMatchingPage &&) = delete;
    ~FriendMatchingPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeOutAnim() override;
    void afterCalc() override;
    void onRefocus() override;

    void prepareStartClient();
    void prepareStartServer();

private:
    class Handler : public SP::RoomManager::Handler {
    public:
        Handler(FriendMatchingPage &page);
        ~Handler();

        void onSetup() override;
        void onMain() override;
        void onTeamSelect() override;
        void onSelect() override;

        void onPlayerJoin(const System::RawMii *mii, u32 location, u16 latitude, u16 longitude,
                u32 regionLineColor) override;
        void onPlayerLeave(u32 playerId) override;
        void onReceiveComment(u32 playerId, u32 messageId) override;
        void onSettingsChange(const std::array<u32, SP::RoomSettings::count> &settings) override;
        void onReceiveTeamSelect(u32 playerId, u32 teamId) override;

    private:
        FriendMatchingPage &m_page;
    };

    void onBack(u32 localPlayerId);
    void onCloseConfirm(s32 choice, PushButton *button);
    void collapse();
    void startClient();
    void startServer();

    template <typename T>
    using H = typename T::Handler<FriendMatchingPage>;

    PageInputManager m_inputManager;
    MatchingMessageWindow m_messageWindow;
    CtrlMenuDummyBack m_dummyBack;
    H<PageInputManager> m_onBack{ this, &FriendMatchingPage::onBack };
    H<YesNoPage> m_onCloseConfirm{ this, &FriendMatchingPage::onCloseConfirm };
    PageId m_replacement;
    Handler m_handler;
    s32 m_gamemode = -1;
    bool m_roomStarted = false;
};

} // namespace UI
