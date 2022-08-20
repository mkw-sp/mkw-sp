#pragma once

#include "game/ui/MatchingMessageWindow.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/ctrl/CtrlMenuDummyBack.hh"

#include <sp/cs/RoomClient.hh>
#include <sp/cs/RoomServer.hh>

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

private:
    class ServerHandler : public SP::RoomServer::Handler {
    public:
        ServerHandler(FriendMatchingPage &page);
        ~ServerHandler();

        void onMain() override;

        void onPlayerJoin(const System::RawMii *mii, u32 location, u16 latitude, u16 longitude)
                override;
        void onPlayerLeave(u32 playerId) override;
        void onReceiveComment(u32 playerId, u32 messageId) override;
        void onSettingsChange(const std::array<u32, SP::RoomSettings::count> &settings) override;

    private:
        FriendMatchingPage &m_page;
    };

    class ClientHandler : public SP::RoomClient::Handler {
    public:
        ClientHandler(FriendMatchingPage &page);
        ~ClientHandler();

        void onSetup() override;
        void onMain() override;

        void onPlayerJoin(const System::RawMii *mii, u32 location, u16 latitude, u16 longitude)
                override;
        void onPlayerLeave(u32 playerId) override;
        void onReceiveComment(u32 playerId, u32 messageId) override;
        void onSettingsChange(const std::array<u32, SP::RoomSettings::count> &settings) override;

    private:
        FriendMatchingPage &m_page;
    };

    void onBack(u32 localPlayerId);
    void onCloseConfirm(s32 choice, PushButton *button);
    void collapse();

    template <typename T>
    using H = typename T::Handler<FriendMatchingPage>;

    PageInputManager m_inputManager;
    MatchingMessageWindow m_messageWindow;
    CtrlMenuDummyBack m_dummyBack;
    H<PageInputManager> m_onBack{ this, &FriendMatchingPage::onBack };
    H<YesNoPage> m_onCloseConfirm{ this, &FriendMatchingPage::onCloseConfirm };
    PageId m_replacement;
    ServerHandler m_serverHandler;
    ClientHandler m_clientHandler;
};

} // namespace UI
