#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/AsyncSocket.hh"
#include "sp/net/ProtoSocket.hh"

#include <game/system/Mii.hh>
#include <game/util/Registry.hh>
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
    void sendComment(u32 commentId);
    void startRoom(u32 gamemode);
    void changeLocalSettings();
    void sendTeamSelect(u32 playerId);
    void sendVote(Registry::Course course, std::optional<Player::Properties> properties);

    static RoomClient *CreateInstance(u32 localPlayerCount, u32 ip, u16 port, u16 passcode);
    static RoomClient *CreateInstance(u32 localPlayerCount, u32 ip, u16 port, LoginInfo loginInfo);
    static void DestroyInstance();
    static RoomClient *Instance();

private:
    // These functions are handled in CreateInstance and DestroyInstance
    RoomClient(u32 localPlayerCount, u32 ip, u16 port, u16 passcode);
    RoomClient(u32 localPlayerCount, u32 ip, u16 port, LoginInfo loginInfo);
    ~RoomClient();

    // Used to update m_state
    std::optional<State> resolve(Handler &handler);
    void transition(Handler &handler, State state);

    // Main state-specific update, used to receive events
    std::expected<State, const wchar_t *> calcConnect();
    std::expected<State, const wchar_t *> calcSetup(Handler &handler);
    std::expected<State, const wchar_t *> calcMain(Handler &handler);
    std::expected<State, const wchar_t *> calcTeamSelect(Handler &h);
    std::expected<State, const wchar_t *> calcSelect(Handler &handler);
    std::expected<void, const wchar_t *> onSetup(Handler &handler);
    void onMain(Handler &handler);
    void onTeamSelect(Handler &handler);
    void onSelect(Handler &handler);

    // Event reading, called from above calc functions
    void onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location, u16 latitude,
            u16 longitude, u32 regionLineColor);
    void onPlayerLeave(Handler &handler, u32 playerId);
    void onReceiveComment(Handler &handler, u32 playerId, u32 messageId);
    void onRoomStart(Handler &handler, u32 gamemode);
    void onReceiveTeamSelect(Handler &handler, u32 playerId, u32 teamId);
    void onReceivePulse(Handler &handler, u32 playerId);
    void onReceiveInfo(Handler &handler, RoomEvent event);

    // Request writing - interface should call these!
    std::expected<void, const wchar_t *> writeJoin();
    std::expected<void, const wchar_t *> writeComment(u32 messageId);
    std::expected<void, const wchar_t *> writeStart(u32 gamemode);
    std::expected<void, const wchar_t *> writeSettings();
    std::expected<void, const wchar_t *> writeTeamSelect(u32 playerId, u32 teamId);
    std::expected<void, const wchar_t *> writeVote(u32 course,
            std::optional<Player::Properties> properties);

    u32 m_localPlayerCount;
    u32 m_localPlayerIds[2];
    bool m_localSettingsChanged = false;
    State m_state;
    Net::AsyncSocket m_innerSocket;
    Net::ProtoSocket<RoomEvent, RoomRequest, Net::AsyncSocket> m_socket;
    u32 m_ip;
    u16 m_port;
    std::optional<LoginInfo> m_loginInfo;
    const wchar_t *m_errorMessage;
    bool m_errored;

    static RoomClient *s_instance;
};

} // namespace SP
