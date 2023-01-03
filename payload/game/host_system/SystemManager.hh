#pragma once

extern "C" {
#include "SystemManager.h"
}
#include "Scene.hh"

#include "game/ui/SectionManager.hh"

#include <sp/IOSDolphin.hh>

namespace System {

class SystemManager {
public:
    void REPLACED(init)();
    REPLACE void init();
    REPLACE void shutdownSystem();
    REPLACE void returnToMenu();
    REPLACE void restart();

    static void ShutdownSystem();
    static void ReturnToMenu();
    static void Restart();
    static void ResetDolphinSpeedLimit();
    static void LaunchTitle(u64 titleID);

private:
    u8 _0000[0x0070 - 0x0000];
    u32 m_launchType;
    u8 _0074[0x1100 - 0x0074];
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
        Connected,   // Success
        Blocked,     // Config::MAIN_USE_DISCORD_PRESENCE is not set.
        Unsupported, // Dolphin version is too old
    };

    ConnectionResult initConnection();
    void terminateConnection();

    void onSectionChange(UI::SectionId sectionId);

private:
    PresenceData &status();
    bool sendStatus();
    void setGameState(std::string_view state, std::string_view details, s64 startTimestamp);

    PresenceData m_cached = s_defaultPresence;
    bool m_statusWasSent = false;
};

} // namespace System
