#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/UnreliableSocket.hh"

namespace SP {

class RaceManager {
public:
    struct Player {
        u32 clientId;
    };

    virtual void destroyInstance() = 0;

    static void OnCreateScene();
    static void OnDestroyScene();
    static RaceManager *Instance();

protected:
    RaceManager();
    // Prevent copy/move
    RaceManager(const RaceManager &) = delete;
    RaceManager(RaceManager &&) = delete;
    ~RaceManager();

    RoomManager &m_roomManager;
    u32 m_playerCount;
    std::array<Player, 12> m_players{};
    u32 m_frameId = 0;

    static void *s_block;
    static RaceManager *s_instance;
};

} // namespace SP
