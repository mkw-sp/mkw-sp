// For mentions of "Track Packs", see `PackSelectPage.cc` for more information.

#pragma once

#include "game/ui/Page.hh"

#include <sp/net/AsyncSocket.hh>
#include <sp/net/ProtoSocket.hh>

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

    void setTrackpack(u32 trackpack) {
        m_trackpack = trackpack;
    }
    void startSearch() {
        m_searchStarted = true;
    }

    std::optional<u16> getVsRating() const {
        return m_vs_rating;
    }
    std::optional<u16> getBtRating() const {
        return m_bt_rating;
    }
    bool isCustomTrackpack() const {
        return m_trackpack != 0;
    }
    State getState() const {
        return m_state;
    }

    std::expected<void, const wchar_t *> transition();
    std::optional<STCMessage_FoundMatch> takeMatchResponse();

    u32 m_gamemode;

private:
    [[nodiscard]] std::expected<void, const wchar_t *> startLogin();
    [[nodiscard]] std::expected<void, const wchar_t *> sendSearchMessage();
    [[nodiscard]] std::expected<void, const wchar_t *> respondToChallenge(const STCMessage &event);
    void setupRatings(const STCMessage &event);
    void setupMatch(const STCMessage &event);

    bool m_searchStarted = false;

    std::optional<u16> m_vs_rating = std::nullopt;
    std::optional<u16> m_bt_rating = std::nullopt;
    u32 m_trackpack;

    std::optional<STCMessage> m_loginResponse;
    std::optional<STCMessage_FoundMatch> m_matchResponse;

    SP::Net::AsyncSocket m_innerSocket;
    SP::Net::ProtoSocket<STCMessage, CTSMessage, SP::Net::AsyncSocket> m_socket;
    PageInputManager m_inputManager;
    State m_state;
};

} // namespace UI
