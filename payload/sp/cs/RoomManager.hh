#pragma once

#include "sp/settings/RoomSettings.hh"

#include <game/system/Mii.hh>

#include <array>

namespace SP {

// RoomManager is inherited by RoomClient and RoomServer
// Its responsibilities are storing metadata and interfacing with UI
class RoomManager {
public:
    // The handler is inherited in networking pages, like FriendMatchingPage and VotingBackPage
    // The server and client will call virtual functions that are overridden by these pages
    class Handler {
    public:
        virtual void onSetup() {}
        virtual void onMain() {}

        virtual void onPlayerJoin([[maybe_unused]] const System::RawMii *mii,
                [[maybe_unused]] u32 location, [[maybe_unused]] u16 latitude,
                [[maybe_unused]] u16 longitude, [[maybe_unused]] u32 regionLineColor) {}
        virtual void onPlayerLeave([[maybe_unused]] u32 playerId) {}
        virtual void onReceiveComment([[maybe_unused]] u32 playerId,
                [[maybe_unused]] u32 commentId) {}
        virtual void onSettingsChange(
                [[maybe_unused]] const std::array<u32, RoomSettings::count> &settings) {}
        virtual void onRoomClose([[maybe_unused]] u32 gamemode) {}
        virtual void onReceivePulse([[maybe_unused]] u32 playerId) {}
        virtual void onReceiveInfo([[maybe_unused]] u32 playerId, [[maybe_unused]] s32 course,
                [[maybe_unused]] u32 selectedPlayer) {}
    };

    class Player {
    public:
        struct Properties {
            u32 m_character;
            u32 m_vehicle;
            bool m_driftIsAuto;
        };

        inline System::RawMii *getMii() {
            return &m_mii;
        }

        u32 m_clientId;
        std::array<u32, RoomSettings::count> m_settings;
        System::RawMii m_mii;
        u32 m_location;
        u32 m_latitude;
        u32 m_longitude;
        u32 m_regionLineColor;
        u32 m_course;
        Properties m_properties;
    };

    inline s32 getGamemode() {
        return m_gamemode;
    }

    inline Player *getPlayer(u8 i) {
        return &m_players[i];
    }

    inline u32 getPlayerCount() {
        return m_playerCount;
    }

    inline s8 getPlayerOrder(u8 i) {
        return m_votePlayerOrder[i];
    }

    // Static instance management is handled in RoomServer and RoomClient
    static void OnCreateScene();
    static void OnDestroyScene();
    static RoomManager *Instance();

protected:
    // Used for RoomServer::Client::m_state and RoomClient::m_state
    enum class ClientState {
        Connect,
        Setup,
        Main,
        Select,
    };

    // Used for RoomServer::m_state
    enum class ServerState {
        Setup,
        Main,
        Select,
    };

    // The ctor and dtor is called in RoomServer/RoomClient's instance management
    RoomManager();
    // Prevent copy/move
    RoomManager(const RoomManager &) = delete;
    RoomManager(RoomManager &&) = delete;
    ~RoomManager();

    u32 m_playerCount = 0;
    s32 m_gamemode = -1;
    std::array<s8, 12> m_votePlayerOrder;
    u8 m_voteCurrentPlayerIdx = 0;
    std::array<Player, 12> m_players;

    static void *s_block;
    static RoomManager *s_instance;
};

} // namespace SP
