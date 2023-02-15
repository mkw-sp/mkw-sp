#pragma once

#include "game/ui/Page.hh"

#include <sp/net/AsyncSocket.hh>

#include <protobuf/Matchmaking.pb.h>

namespace UI {

class OnlineConnectionManagerPage : public Page {
public:
    enum class State {
        Initial,
        WaitForLoginChallenge,
        WaitForLoginResponse,
        WaitForSearchStart,
        WaitForSearchResponse,
        FoundMatch,
    };

    OnlineConnectionManagerPage();
    void onInit() override;
    void afterCalc() override;

    void setTrackpack(u32 trackpack) { m_trackpack = trackpack; }
    void startSearch() { m_searchStarted = true; }
    void setGamemode(u32 gamemode);

    bool isCustomTrackpack() const { return m_trackpack != 0; }
    State getState() const { return m_state; }

    std::optional<STCMessage> takeLoginResponse();
    std::optional<STCMessage_FoundMatch> takeMatchResponse();
private:
    bool read(std::optional<STCMessage> &event);
    bool write(CTSMessage message);

    void startLogin();
    void sendSearchMessage();
    void respondToChallenge(const STCMessage &event);
    void setupMatch(const STCMessage &event);

    bool m_searchStarted = false;
    u32 m_trackpack;
    u32 m_gamemode;

    std::optional<STCMessage> m_loginResponse;
    std::optional<STCMessage_FoundMatch> m_matchResponse;

    PageInputManager m_inputManager;
    SP::Net::AsyncSocket m_socket;
    State m_state;
};

} // namespace UI
