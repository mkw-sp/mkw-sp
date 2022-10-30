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
        virtual void onTeamSelect() {}
        virtual void onSelect() {}

        virtual void onPlayerJoin([[maybe_unused]] const System::RawMii *mii,
                [[maybe_unused]] u32 location, [[maybe_unused]] u16 latitude,
                [[maybe_unused]] u16 longitude, [[maybe_unused]] u32 regionLineColor) {}
        virtual void onPlayerLeave([[maybe_unused]] u32 playerId) {}
        virtual void onReceiveComment([[maybe_unused]] u32 playerId,
                [[maybe_unused]] u32 commentId) {}
        virtual void onSettingsChange(
                [[maybe_unused]] const std::array<u32, RoomSettings::count> &settings) {}
        virtual void onReceiveTeamSelect([[maybe_unused]] u32 playerId,
                [[maybe_unused]] u32 teamId) {}
        virtual void onReceivePulse([[maybe_unused]] u32 playerId) {}
        virtual void onReceiveInfo([[maybe_unused]] u32 playerId, [[maybe_unused]] s32 course,
                [[maybe_unused]] u32 selectedPlayer) {}
    };

    // TODO cleanup
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
        u32 m_teamId;
    };

    // TODO remove
    inline Player *getPlayer(u8 i) {
        return &m_players[i];
    }

    // TODO remove
    inline u32 getPlayerCount() {
        return m_playerCount;
    }

    // TODO remove
    inline s8 getPlayerOrder(u8 i) {
        return m_votePlayerOrder[i];
    }

    // TODO remove
    inline s32 getGamemode() {
        return m_gamemode;
    }

    u32 playerCount() const;
    const Player& player(u32 playerId) const;
    u32 gamemode() const;

    // TODO remove get prefix
    template <SP::ClientSettings::Setting S>
    SP::ClientSettings::Helper<S>::type getSetting() const {
        static_assert(static_cast<u32>(S) >= RoomSettings::offset);
        constexpr u32 setting = static_cast<u32>(S) - RoomSettings::offset;
        static_assert(setting < RoomSettings::count);
        return static_cast<SP::ClientSettings::Helper<S>::type>(settings()[setting]);
    }

    virtual bool isPlayerLocal(u32 playerId) const = 0;
    virtual bool canSelectTeam(u32 playerId) const = 0;
    virtual bool canSelectTeam(u32 localPlayerId, u32 playerId) const = 0;

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
        TeamSelect,
        Select,
    };

    // Used for RoomServer::m_state
    enum class ServerState {
        Setup,
        Main,
        TeamSelect,
        Select,
    };

    // The ctor and dtor is called in RoomServer/RoomClient's instance management
    RoomManager();
    // Prevent copy/move
    RoomManager(const RoomManager &) = delete;
    RoomManager(RoomManager &&) = delete;
    ~RoomManager();

    virtual const std::array<u32, RoomSettings::count> &settings() const = 0;

    u32 m_playerCount = 0;
    std::array<Player, 12> m_players;
    u32 m_gamemode = 0;
    std::array<s8, 12> m_votePlayerOrder;
    u8 m_voteCurrentPlayerIdx = 0;

    static void *s_block;
    static RoomManager *s_instance;
};

} // namespace SP
