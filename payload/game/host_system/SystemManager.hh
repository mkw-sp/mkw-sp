#pragma once

#include "game/ui/SectionId.hh"

#include <egg/core/eggHeap.hh>
#include <sp/IOSDolphin.hh>

namespace System {

class SystemManager {
public:
    void REPLACED(init)();
    REPLACE void init();
    REPLACE void shutdownSystem();
    REPLACE void returnToMenu();
    REPLACE void restart();
    u32 aspectRatio() const;
    u32 matchingArea() const;
    u32 launchType() const;

    static void ShutdownSystem();
    static void ReturnToMenu();
    static void Restart();
    static void ResetDolphinSpeedLimit();
    static void LaunchTitle(u64 titleID);
    static u8 *RipFromDisc(const char *path, EGG::Heap *heap, bool allocTop, u32 *size);

    static SystemManager *Instance() {
        return s_instance;
    }

private:
    u8 _0000[0x0058 - 0x0000];
    u32 m_aspectRatio;
    u8 _005c[0x0070 - 0x005c];
    u32 m_launchType;
    u8 _0074[0x0084 - 0x0074];
    u32 m_matchingArea;
    u8 _0088[0x1100 - 0x0088];

    static SystemManager *s_instance;
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
