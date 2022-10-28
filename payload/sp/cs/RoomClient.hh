#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/AsyncSocket.hh"
#include "sp/settings/RoomSettings.hh"

#include <game/system/Mii.hh>
#include <protobuf/Room.pb.h>

#include <array>

namespace SP {

// RoomClient is responsible for writing requests and reading events
class RoomClient final : public RoomManager {
public:
    // Main RoomClient update, called in networking pages (typically afterCalc())
    bool calc(Handler &handler);

    // Request writing interface - new requests should go here!
    bool sendComment(u32 commentId);
    bool closeRoom(u32 gamemode);
    void changeLocalSettings();
    bool sendVote(u32 course, std::optional<Player::Properties> properties);

    static RoomClient *CreateInstance(u32 localPlayerCount);
    static void DestroyInstance();
    static RoomClient *Instance();

private:
    // These functions are handled in CreateInstance and DestroyInstance
    RoomClient(u32 localPlayerCount);
    ~RoomClient();

    // Used to update m_state
    std::optional<ClientState> resolve(Handler &handler);
    bool transition(Handler &handler, ClientState state);

    // Main state-specific update, used to receive events
    std::optional<ClientState> calcConnect();
    std::optional<ClientState> calcSetup(Handler &handler);
    std::optional<ClientState> calcMain(Handler &handler);
    std::optional<ClientState> calcSelect(Handler &handler);
    bool onSetup(Handler &handler);
    bool onMain(Handler &handler);

    // Event reading, called from above calc functions
    bool onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location, u16 latitude,
            u16 longitude, u32 regionLineColor);
    bool onPlayerLeave(Handler &handler, u32 playerId);
    bool onReceiveComment(Handler &handler, u32 playerId, u32 messageId);
    bool onRoomClose(Handler &handler, u32 gamemode);
    bool onReceivePulse(Handler &handler, u32 playerId);
    bool onReceiveInfo(Handler &handler, RoomEvent event);

    // Request writing - interface should call these!
    bool read(std::optional<RoomEvent> &event);
    bool writeJoin();
    bool writeComment(u32 messageId);
    bool writeClose(u32 gamemode);
    bool writeSettings();
    bool writeVote(u32 course, std::optional<Player::Properties> properties);
    bool write(RoomRequest request);

    u32 m_localPlayerCount;
    u32 m_localPlayerIds[2];
    bool m_localSettingsChanged = false;
    ClientState m_state;
    Net::AsyncSocket m_socket;

    static RoomClient *s_instance;
};

} // namespace SP
