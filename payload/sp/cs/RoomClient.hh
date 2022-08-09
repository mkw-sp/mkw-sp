#pragma once

#include "sp/net/AsyncSocket.hh"

#include <game/system/Mii.hh>
#include <protobuf/Room.pb.h>

#include <array>

namespace SP {

class RoomClient final {
public:
    class Handler {
    public:
        virtual void onSetup() {}
        virtual void onMain() {}

        virtual void onPlayerJoin([[maybe_unused]] const System::RawMii *mii,
                [[maybe_unused]] u32 location, [[maybe_unused]] u16 latitude,
                [[maybe_unused]] u16 longitude) {}
        virtual void onPlayerLeave([[maybe_unused]] u32 playerId) {}
    };

    bool calc(Handler &handler);

    static void OnCreateScene();
    static void OnDestroyScene();
    static RoomClient *CreateInstance(u32 localPlayerCount);
    static void DestroyInstance();
    static RoomClient *Instance();

private:
    enum class State {
        Connect,
        Setup,
        Main,
    };

    RoomClient(u32 localPlayerCount);
    RoomClient(const RoomClient &) = delete;
    RoomClient(RoomClient &&) = delete;
    ~RoomClient();

    std::optional<State> resolve(Handler &handler);
    bool transition(Handler &handler, State state);

    std::optional<State> calcConnect();
    std::optional<State> calcSetup(Handler &handler);
    std::optional<State> calcMain(Handler &handler);
    bool onSetup(Handler &handler);
    bool onMain(Handler &handler);

    bool onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location, u16 latitude,
            u16 longitude);
    bool onPlayerLeave(Handler &handler, u32 playerId);

    bool read(std::optional<RoomEvent> &event);
    bool writeJoin();
    bool write(RoomRequest request);

    u32 m_localPlayerCount;
    u32 m_localPlayerIds[2];
    u32 m_playerCount = 0;
    State m_state;
    Net::AsyncSocket m_socket;

    static void *s_block;
    static RoomClient *s_instance;
};

} // namespace SP
