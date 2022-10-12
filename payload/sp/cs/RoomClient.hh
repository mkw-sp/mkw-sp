#pragma once

#include "sp/net/AsyncSocket.hh"
#include "sp/settings/RoomSettings.hh"

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
                [[maybe_unused]] u16 longitude, [[maybe_unused]] u32 regionLineColor) {}
        virtual void onPlayerLeave([[maybe_unused]] u32 playerId) {}
        virtual void onReceiveComment([[maybe_unused]] u32 playerId,
                [[maybe_unused]] u32 commentId) {}
        virtual void onSettingsChange(
                [[maybe_unused]] const std::array<u32, RoomSettings::count> &settings) {}
        virtual void onRoomClose([[maybe_unused]] u32 gamemode) {}
        virtual void onSelect([[maybe_unused]] u32 playerId) {}
    };

    bool calc(Handler &handler);
    bool sendComment(u32 commentId);
    bool closeRoom(u32 gamemode);
    void changeLocalSettings();
    bool sendVote(u32 course, std::optional<u32> characterId, std::optional<u32> vehicleId, std::optional<bool> driftIsAuto);

    inline u32 getPlayerCount() { return m_playerCount; }

    static void OnCreateScene();
    static void OnDestroyScene();
    static RoomClient *CreateInstance(u32 localPlayerCount);
    static void DestroyInstance();
    static RoomClient *Instance();

private:
    struct PlayerProperties {
        u32 characterId;
        u32 vehicleId;
        bool driftIsAuto;
    };

    enum class State {
        Connect,
        Setup,
        Main,
        Select,
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
    std::optional<State> calcSelect(Handler &handler);
    bool onSetup(Handler &handler);
    bool onMain(Handler &handler);

    bool onPlayerJoin(Handler &handler, const System::RawMii *mii, u32 location, u16 latitude,
            u16 longitude, u32 regionLineColor);
    bool onPlayerLeave(Handler &handler, u32 playerId);
    bool onReceiveComment(Handler &handler, u32 playerId, u32 messageId);
    bool onRoomClose(Handler &handler, u32 gamemode);
    bool onSelect(Handler &handler, u32 playerId);

    bool read(std::optional<RoomEvent> &event);
    bool writeJoin();
    bool writeComment(u32 messageId);
    bool writeClose(u32 gamemode);
    bool writeSettings();
    bool writeVote(u32 course, std::optional<PlayerProperties> properties);
    bool write(RoomRequest request);

    u32 m_localPlayerCount;
    u32 m_localPlayerIds[2];
    bool m_localSettingsChanged = false;
    u32 m_playerCount = 0;
    std::array<u32, RoomSettings::count> m_settings;
    State m_state;
    Net::AsyncSocket m_socket;

    static void *s_block;
    static RoomClient *s_instance;
};

} // namespace SP
