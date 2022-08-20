#pragma once

#include "game/ui/FriendMatchingPlayer.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/CircularBuffer.hh>

#include <variant>

namespace UI {

class FriendRoomBackPage : public Page {
public:
    FriendRoomBackPage();
    FriendRoomBackPage(const FriendRoomBackPage &) = delete;
    FriendRoomBackPage(FriendRoomBackPage &&) = delete;
    ~FriendRoomBackPage() override;

    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void afterCalc() override;
    void onRefocus() override;

    void pop();
    void onPlayerJoin(System::RawMii mii, u32 location, u16 latitude, u16 longitude);
    void onPlayerLeave(u32 playerId);

private:
    struct Join {
        System::RawMii mii;
        u32 location;
        u16 latitude;
        u16 longitude;
    };

    struct Leave {
        u32 playerId;
    };

    MenuInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    FriendMatchingPlayer m_players[12];
    MiiGroup m_miiGroup;
    u32 m_playerCount;
    u32 m_indices[12];
    std::optional<u32> m_globePlayerId;
    u32 m_timer;
    SP::CircularBuffer<std::variant<Join, Leave>, 24> m_queue;
};

} // namespace UI
