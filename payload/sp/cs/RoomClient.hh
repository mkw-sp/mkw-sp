#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/AsyncSocket.hh"

#include <game/system/Mii.hh>
#include <protobuf/Room.pb.h>

#include <array>

namespace SP {

// RoomClient is responsible for writing requests and reading events
class RoomClient final : public RoomManager {
public:
    using State = RoomManager::ClientState;

    bool isPlayerLocal(u32 playerId) const override;
    bool isPlayerRemote(u32 playerId) const override;
    bool canSelectTeam(u32 playerId) const override;
    bool canSelectTeam(u32 localPlayerId, u32 playerId) const override;

    void destroyInstance() override;
    // Main RoomClient update, called in networking pages (typically afterCalc())
    bool calc(Handler &handler) override;

    u32 ip() const;
    u16 port() const;
    hydro_kx_session_keypair keypair() const;
    Net::AsyncSocket &socket();

    // Request writing interface - new requests should go here!
    // TODO these should return void and defer the actual sending
    bool sendComment(u32 commentId);
    bool startRoom(u32 gamemode);
    void changeLocalSettings();
    bool sendTeamSelect(u32 playerId);
    bool sendVote(u32 course, std::optional<Player::Properties> properties);

    static RoomClient *CreateInstance(u32 localPlayerCount, u32 ip, u16 port, u16 passcode, std::optional<LoginInfo> loginInfo);
    static void DestroyInstance();
    static RoomClient *Instance();

private:
    // These functions are handled in CreateInstance and DestroyInstance
    RoomClient(u32 localPlayerCount, u32 ip, u16 port, u16 passcode, std::optional<LoginInfo> loginInfo);
    ~RoomClient();

    // Used to update m_state
    std::optional<State> resolve(Handler &handler);
    bool transition(Handler &handler, State state);

    // Main state-specific update, used to receive events
    std::optional<State> calcConnect();
    std::optional<State> calcSetup(Handler &handler);
    std::optional<State> calcMain(Handler &handler);
    std::optional<State> calcTeamSelect(Handler &handler);
    std::optional<State> calcSelect(Handler &handler);
    bool onSetup(Handler &handler);
    bool onMain(Handler &handler);
    bool onTeamSelect(Handler &handler);
    bool onSelect(Handler &handler);

    // Event reading, called from above calc functions
    bool onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location, u16 latitude,
            u16 longitude, u32 regionLineColor);
    bool onPlayerLeave(Handler &handler, u32 playerId);
    bool onReceiveComment(Handler &handler, u32 playerId, u32 messageId);
    bool onRoomStart(Handler &handler, u32 gamemode);
    bool onReceiveTeamSelect(Handler &handler, u32 playerId, u32 teamId);
    bool onReceivePulse(Handler &handler, u32 playerId);
    bool onReceiveInfo(Handler &handler, RoomEvent event);

    // Request writing - interface should call these!
    bool read(std::optional<RoomEvent> &event);
    bool writeJoin();
    bool writeComment(u32 messageId);
    bool writeStart(u32 gamemode);
    bool writeSettings();
    bool writeTeamSelect(u32 playerId, u32 teamId);
    bool writeVote(u32 course, std::optional<Player::Properties> properties);
    bool write(RoomRequest request);

    u32 m_localPlayerCount;
    u32 m_localPlayerIds[2];
    bool m_localSettingsChanged = false;
    State m_state;
    Net::AsyncSocket m_socket;
    u32 m_ip;
    u16 m_port;
    std::optional<LoginInfo> m_loginInfo;

    static RoomClient *s_instance;
};

} // namespace SP
