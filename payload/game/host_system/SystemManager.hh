#pragma once

extern "C" {
#include "SystemManager.h"
}

#include <sp/IOSDolphin.hh>
#include "Scene.hh"

namespace System {

class SystemManager {
public:
    REPLACE void shutdownSystem();
    REPLACE void returnToMenu();
    REPLACE void restart();

    static void ShutdownSystem();
    static void ReturnToMenu();
    static void Restart();
    static void ResetDolphinSpeedLimit();
    static void LaunchTitle(u64 titleID);

private:
    u8 _0000[0x1100 - 0x0000];
};
static_assert(sizeof(SystemManager) == 0x1100);

class RichPresenceManager {
public:
    using PresenceData = SP::IOSDolphin::DolphinDiscordPresence;

    RichPresenceManager() = default;
    RichPresenceManager(const RichPresenceManager &) = default;
    RichPresenceManager(RichPresenceManager &&) = default;
    ~RichPresenceManager() = default;

    static RichPresenceManager &Instance();
    static void Init();

    static constexpr std::string_view s_applicationId = "1006667953393111040";
    static constexpr PresenceData s_defaultPresence = {
        .state = "Main Menu",
        .details = "MKW-SP",
        .startTimestamp = 0,
        .endTimestamp = 0,
        .largeImageKey = "logo",
        .largeImageText = "MKW-SP",
        .smallImageKey = "logo",
        .smallImageText = "MKW-SP",
        .partySize = 0,
        .partyMax = 0,
    };

    enum class ConnectionResult {
        Connected,    // Success
        Blocked,      // Config::MAIN_USE_DISCORD_PRESENCE is not set.
        Unsupported,  // Dolphin version is too old
    };

    ConnectionResult initConnection();
    void terminateConnection();

    void setGameState(std::string_view state) {
        if (!state.empty() &&
                (!m_statusWasSent || state != std::string_view(status().state))) {
            status().state = state;
            sendStatus();
        }
    }

    // Internal
    PresenceData &status();
    bool sendStatus();

    // TODO: Something more advanced, look at Section IDs
    void onSceneChange(RKSceneID id) {
        switch (id) {
        case RKSceneID::Title:
        case RKSceneID::Menu:
            setGameState("Main Menu");
            break;
        case RKSceneID::Race:
            setGameState("In a Race");
            break;
        case RKSceneID::Globe:
            setGameState("Online Menu");
            break;
        case RKSceneID::Strap:
        case RKSceneID::FlagOpen:
            break;
        }
    }

private:
    PresenceData m_cached = s_defaultPresence;
    bool m_statusWasSent = false;
};

} // namespace System
