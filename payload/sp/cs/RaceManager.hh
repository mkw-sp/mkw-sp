#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/UnreliableSocket.hh"

namespace SP {

class RaceManager {
public:
    class Player {
        // TODO: figure out what we need
    };

    virtual void destroyInstance() = 0;

    static void OnCreateScene();
    static void OnDestroyScene();

protected:
    RaceManager();
    // Prevent copy/move
    RaceManager(const RaceManager &) = delete;
    RaceManager(RaceManager &&) = delete;
    ~RaceManager();

    RoomManager &m_roomManager;

    static void *s_block;
    static RaceManager *s_instance;
};

} // namespace SP
