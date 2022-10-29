#pragma once

#include "sp/cs/RoomManager.hh"
#include "sp/net/AsyncListener.hh"
#include "sp/net/AsyncSocket.hh"
#include "sp/settings/RoomSettings.hh"

#include <game/system/Mii.hh>
#include <protobuf/Room.pb.h>

#include <array>
#include <bitset>

namespace SP {

// RoomServer is primarily responsible for reading requests and writing events
// Its secondary responsibility is keeping track of comments in friend rooms
class RoomServer final : public RoomManager {
public:
    using State = RoomManager::ServerState;
    // Main RoomServer update, called in networking pages (typically afterCalc())
    bool calc(Handler &handler);

    static RoomServer *CreateInstance();
    static void DestroyInstance();
    static RoomServer *Instance();

private:
    struct Comment {
        u32 playerId;
        u32 messageId;
    };

    // RoomServer::Client is responsible for sending events to individual clients
    class Client {
    public:
        Client(RoomServer &server, u32 id, s32 handle, const hydro_kx_keypair &serverKeypair);
        ~Client();

        bool ready() const;
        bool calc(Handler &handler);

        // Event writing for clients
        bool writeJoin(const System::RawMii *mii, u32 location, u32 latitude, u32 longitude,
                u32 regionLineColor);
        bool writeLeave(u32 playerId);
        bool writeComment(u32 playerId, u32 messageId);
        bool writeSettings();
        bool writeClose(u32 gamemode);
        bool writeSelectPulse(u32 playerId);
        bool writeSelectInfo(u32 selectedPlayer);

    private:
        // Used to update m_state
        std::optional<ClientState> resolve(Handler &handler);
        bool transition(Handler &handler, ClientState state);

        // Main state-specific update, used to receive requests
        std::optional<ClientState> calcConnect();
        std::optional<ClientState> calcSetup(Handler &handler);
        std::optional<ClientState> calcMain(Handler &handler);
        std::optional<ClientState> calcSelect(Handler &handler);

        bool isHost() const;
        std::optional<u32> getPlayerId() const;

        bool read(std::optional<RoomRequest> &request);
        bool write(RoomEvent event);

        u32 m_id;
        RoomServer &m_server;
        ClientState m_state;
        Net::AsyncSocket m_socket;
    };

    RoomServer();
    ~RoomServer();

    // Used to update m_state
    std::optional<State> resolve(Handler &handler);
    bool transition(Handler &handler, State state);

    // Main state-specific update, used to send events
    std::optional<State> calcSetup();
    std::optional<State> calcMain(Handler &handler);
    std::optional<State> calcSelect(Handler &handler);
    bool onMain(Handler &handler);

    // Server-side request reading - any validation checks should go here!
    bool onPlayerJoin(Handler &handler, u32 clientId, const System::RawMii *mii, u32 location,
            u16 latitude, u16 longitude, u32 regionLineColor,
            const std::array<u32, RoomSettings::count> &settings);
    void onPlayerLeave(Handler &handler, u32 playerId);
    bool onReceiveComment(u32 playerId, u32 messageId);
    bool onRoomClose(u32 playerId, s32 gamemode);
    bool onReceiveVote(u32 playerId, u32 course, Player::Properties &properties);
    bool validateProperties(u32 playerId, Player::Properties &properties);

    void disconnectClient(u32 clientId);

    // Event writing - should usually consist of a for loop for all clients!
    void writeJoin(const System::RawMii *mii, u32 location, u32 latitude, u32 longitude,
            u32 regionLineColor);
    void writeLeave(u32 playerId);
    void writeComment(u32 playerId, u32 messageId);
    void writeSettings();
    void writeClose(u32 gamemode);
    void writeSelectPulse(u32 playerId);
    void writeSelectInfo(u32 selectedPlayer);

    CircularBuffer<Comment, 18> m_commentQueue;
    u32 m_commentTimer = 0;
    bool m_settingsChanged = false;
    bool m_roomClosed = false;
    bool m_voteEvent = false;
    std::bitset<12> m_voted;
    u8 m_voteCount = 0;
    s8 m_voteDelay = 15;
    State m_state;
    CircularBuffer<u32, 12> m_disconnectQueue;
    std::array<std::optional<Client>, 12> m_clients;
    Net::AsyncListener m_listener;
    hydro_kx_keypair m_keypair;

    static RoomServer *s_instance;
};

} // namespace SP
