// For mentions of "Track Packs", see `PackSelectPage.cc` for more information.

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

    void startSearch() {
        m_searchStarted = true;
    }

    std::optional<u16> getVsRating() const {
        return m_vs_rating;
    }
    std::optional<u16> getBtRating() const {
        return m_bt_rating;
    }
    State getState() const {
        return m_state;
    }

    std::optional<STCMessage_FoundMatch> takeMatchResponse();

    u32 m_gamemode;

private:
    bool read(std::optional<STCMessage> &event);
    void write(CTSMessage message);

    void startLogin();
    void sendSearchMessage();
    void respondToChallenge(const STCMessage &event);
    void setupRatings(const STCMessage &event);
    void setupMatch(const STCMessage &event);

    bool m_searchStarted = false;

    std::optional<u16> m_vs_rating = std::nullopt;
    std::optional<u16> m_bt_rating = std::nullopt;

    std::optional<STCMessage> m_loginResponse;
    std::optional<STCMessage_FoundMatch> m_matchResponse;

    PageInputManager m_inputManager;
    SP::Net::AsyncSocket m_socket;
    State m_state;
};

} // namespace UI
