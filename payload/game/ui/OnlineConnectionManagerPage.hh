#pragma once

#include "game/ui/Page.hh"

#include <sp/CircularBuffer.hh>
#include <sp/net/AsyncSocket.hh>

#include <protobuf/Matchmaking.pb.h>

namespace UI {

class OnlineConnectionManagerPage : public Page {
public:
    enum class State {
        Initial,
        WaitForLoginChallenge,
        WaitForLoginResponse,
        Idle,
        FoundMatch,
    };

    OnlineConnectionManagerPage();

    void onInit() override;
    void afterCalc() override;

    void requestRegisterFriend(u32 deviceId, u8 licenceId, bool isDelete);
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

    static u32 GetDeviceId();
    u64 getFriendCode(u32 deviceId, u8 licenceId);
    size_t getFriendCount() const {
        return m_friends.count();
    }
    const STCMessage_UpdateFriendsList_Friend *getFriend(size_t idx) const {
        return m_friends[idx];
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
    void handleLoginResponse(const STCMessage &event);
    void handleFriendsList(const STCMessage_UpdateFriendsList &event);
    void handleFriendRequestResponse(const STCMessage &event);
    void setupMatch(const STCMessage &event);

    bool m_searchStarted = false;

    SP::CircularBuffer<STCMessage_UpdateFriendsList_Friend, 30> m_friends;
    std::optional<u16> m_vs_rating = std::nullopt;
    std::optional<u16> m_bt_rating = std::nullopt;
    u32 m_trackpack;

    std::optional<STCMessage> m_loginResponse;
    std::optional<STCMessage_FoundMatch> m_matchResponse;

    PageInputManager m_inputManager;
    SP::Net::AsyncSocket m_socket;
    State m_state;
};

} // namespace UI
