#pragma once

#include "game/ui/FriendMatchingPlayer.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/CircularBuffer.hh>
#include <sp/settings/RoomSettings.hh>

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
    void onReceiveComment(u32 playerId, u32 messageId);
    void onSettingsChange(const std::array<u32, SP::RoomSettings::count> &settings);
    void onRoomClose(u32 gamemode);

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

    struct Comment {
        u32 playerId;
        u32 messageId;
    };

    struct Settings {
        std::array<u32, SP::RoomSettings::count> settings;
    };

    struct Close {
        u32 messageId;
    };

    MenuInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    FriendMatchingPlayer m_players[12];
    MiiGroup m_miiGroup;
    u32 m_locations[12];
    u16 m_latitudes[12];
    u16 m_longitudes[12];
    u32 m_playerCount;
    u32 m_indices[12];
    std::optional<u32> m_globePlayerId;
    u32 m_timer;
    // Join: 12
    // Leave: 12
    // Comment: 18
    // Settings: 1
    // Close: 1
    SP::CircularBuffer<std::variant<Join, Leave, Comment, Settings, Close>, 44> m_queue;
    bool m_roomClosed = false;
};

} // namespace UI
