#include "SystemManager.hh"

#include "game/system/RaceManager.hh"

extern "C" {
#include <revolution.h>
}

namespace System {

void SystemManager::shutdownSystem() {
    ShutdownSystem();
}

void SystemManager::returnToMenu() {
    ReturnToMenu();
}

void SystemManager::restart() {
    Restart();
}

void SystemManager::ShutdownSystem() {
    ResetDolphinSpeedLimit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    OSShutdownSystem();
}

void SystemManager::ReturnToMenu() {
    ResetDolphinSpeedLimit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    OSDisableScheduler();
    __OSShutdownDevices(6);
    OSEnableScheduler();
    __OSReturnToMenuForError();
}

void SystemManager::Restart() {
    ResetDolphinSpeedLimit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    OSDisableScheduler();
    __OSShutdownDevices(6);
    OSEnableScheduler();
    LaunchTitle(CHANNEL_TITLE_ID);
    LaunchTitle(UINT64_C(0x000100014f484243) /* OHBC */);
    LaunchTitle(UINT64_C(0x000100014c554c5a) /* LULZ */);
    LaunchTitle(UINT64_C(0x0000000100000002));
}

void SystemManager::ResetDolphinSpeedLimit() {
    if (SP::IOSDolphin::Open()) {
        SP::IOSDolphin::SetSpeedLimit(100);
    }
}

void SystemManager::LaunchTitle(u64 titleID) {
    if (!OSIsTitleInstalled(titleID)) {
        return;
    }
    __OSLaunchTitle(titleID);
}

RichPresenceManager &RichPresenceManager::Instance() {
    static RichPresenceManager manager;
    return manager;
}

void RichPresenceManager::Init() {
    auto &manager = Instance();
    switch (manager.initConnection()) {
    case ConnectionResult::Connected:
        SP_LOG("Discord RPC enabled");
        manager.sendStatus();
        break;
    case ConnectionResult::Blocked:
        SP_LOG("Discord RPC could not be enabled because Config::MAIN_USE_DISCORD_PRESENCE is not "
               "set");
        break;
    case ConnectionResult::Unsupported:
        break;
    }
}

RichPresenceManager::ConnectionResult RichPresenceManager::initConnection() {
    m_statusWasSent = false;
    if (!SP::IOSDolphin::Open()) {
        return ConnectionResult::Unsupported;
    }

    const IPCResult ec = SP::IOSDolphin::DiscordSetClient(s_applicationId);
    switch (ec) {
    case IPC_OK:
        return ConnectionResult::Connected;
    case IPC_EACCES:
        return ConnectionResult::Blocked;
    case IPC_EINVAL:
    default:
        return ConnectionResult::Unsupported;
    }
}
void RichPresenceManager::terminateConnection() {
    m_statusWasSent = false;
    if (SP::IOSDolphin::Open()) {
        SP::IOSDolphin::DiscordReset();
    }
}

void RichPresenceManager::onSectionChange(UI::SectionId sectionId) {
    std::string_view state;
    std::string_view details = "MKW-SP";
    s64 startTimestamp = 0;
    RKSceneID sceneId = static_cast<RKSceneID>(UI::Section::GetSceneId(sectionId));

    switch (sectionId) {
    case UI::SectionId::Thumbnails:
        state = "Generating thumbnails...";
        break;
    case (UI::SectionId)0x10:
        state = "Creating new save...";
        break;
    case (UI::SectionId)0x11:
    case (UI::SectionId)0x12:
    case (UI::SectionId)0x13:
    case (UI::SectionId)0x14:
    case (UI::SectionId)0x15:
        state = "Error on launch!";
        break;
    case (UI::SectionId)0x19:
        state = "Starting Grand Prix...";
        break;
    case (UI::SectionId)0x1A:
        state = "Starting VS Race...";
        break;
    case (UI::SectionId)0x1B:
        state = "Starting Battle Mode...";
        break;
    case (UI::SectionId)0x1C:
        state = "Starting Mission Mode...";
        break;
    case (UI::SectionId)0x1D:
        state = "Starting competition...";
        break;
    case UI::SectionId::GP:
        state = "Playing a Grand Prix!";
        break;
    case UI::SectionId::TA:
    case UI::SectionId::GhostTA:
    case UI::SectionId::GhostTAOnline:
        state = "Playing a Time Trial!";
        break;
    case UI::SectionId::VS1P:
        state = "Playing a VS Race!";
        break;
    case UI::SectionId::VS2P:
    case UI::SectionId::VS3P:
    case UI::SectionId::VS4P:
        state = "Playing a multiplayer VS Race!";
        break;
    case UI::SectionId::TeamVS1P:
        state = "Playing a team VS Race!";
        break;
    case UI::SectionId::TeamVS2P:
    case UI::SectionId::TeamVS3P:
    case UI::SectionId::TeamVS4P:
        state = "Playing a multiplayer team VS Race!";
        break;
    case UI::SectionId::Battle1P:
        state = "Playing a battle!";
        break;
    case UI::SectionId::Battle2P:
    case UI::SectionId::Battle3P:
    case UI::SectionId::Battle4P:
        state = "Playing a multiplayer battle!";
        break;
    case UI::SectionId::MRReplay:
        state = "Watching a mission replay!";
        break;
    case UI::SectionId::TournamentReplay:
        state = "Watching a competition replay!";
        break;
    case UI::SectionId::GPReplay:
        state = "Watching a Grand Prix replay!";
        break;
    case UI::SectionId::TAReplay:
        state = "Watching a Time Trial replay!";
        break;
    case UI::SectionId::GhostReplayChannel:
    case UI::SectionId::GhostReplayDownload:
    case UI::SectionId::GhostReplay:
        state = "Watching a ghost replay!";
        break;
    case UI::SectionId::AwardsGP:
    case UI::SectionId::AwardsVS:
    case (UI::SectionId)0x37:
    case (UI::SectionId)0x38:
        state = "In an awards ceremony!";
        break;
    case (UI::SectionId)0x39:
    case (UI::SectionId)0x3A:
    case (UI::SectionId)0x3B:
    case (UI::SectionId)0x3C:
    case (UI::SectionId)0x3D:
    case (UI::SectionId)0x3E:
        state = "Playing credits!";
        break;
    case (UI::SectionId)0x3F:
    case (UI::SectionId)0x40:
    case UI::SectionId::TitleFromMenu:
    case (UI::SectionId)0x42:
    case UI::SectionId::TitleFromOptions:
        state = "Selecting license/game type...";
        break;
    case (UI::SectionId)0x44:
        state = "In a... demo?";
        break;
    case UI::SectionId::MiiSelectCreate:
    case UI::SectionId::MiiSelectChange:
    case UI::SectionId::LicenseSettings:
        state = "Changing license settings...";
        break;
    case UI::SectionId::Single:
    case UI::SectionId::SingleChangeDriver:
    case UI::SectionId::SingleChangeCourse:
    case UI::SectionId::SingleChooseVSCourse:
    case UI::SectionId::SingleChooseBattleCourse:
    case UI::SectionId::SingleChangeMission:
    case UI::SectionId::SingleChangeGhostData:
    case (UI::SectionId)0x4F:
    case (UI::SectionId)0x50:
        state = "Configuring singleplayer race...";
        break;
    case UI::SectionId::Multi:
        state = "Configuring multiplayer race...";
        break;
    case UI::SectionId::OnlineSingle:
    case (UI::SectionId)0x56:
    case (UI::SectionId)0x57:
    case UI::SectionId::OnlineMultiConfigure:
    case UI::SectionId::OnlineMulti:
    case (UI::SectionId)0x5C:
    case (UI::SectionId)0x5D:
        state = "Searching for a room...";
        break;
    case (UI::SectionId)0x58:
    case (UI::SectionId)0x59:
    case (UI::SectionId)0x5E:
    case (UI::SectionId)0x5F:
    case UI::SectionId::Voting1PVS:
    case UI::SectionId::Voting1PBalloon:
    case UI::SectionId::Voting1PCoin:
    case UI::SectionId::Voting2PVS:
    case UI::SectionId::Voting2PBalloon:
    case UI::SectionId::Voting2PCoin:
        state = "Voting for online track!";
        break;
    case UI::SectionId::VotingServer:
        state = "Selecting online track...";
        details = "Server Mode";
        break;
    case (UI::SectionId)0x68:
    case (UI::SectionId)0x69:
    case UI::SectionId::Online1PVS:
    case (UI::SectionId)0x71:
    case (UI::SectionId)0x74:
    case (UI::SectionId)0x75:
        state = "Playing in an online VS race!";
        break;
    case (UI::SectionId)0x6A:
    case (UI::SectionId)0x6B:
        state = "Spectating an online race!";
        break;
    case (UI::SectionId)0x6C:
    case (UI::SectionId)0x6D:
    case (UI::SectionId)0x72:
    case (UI::SectionId)0x73:
    case (UI::SectionId)0x76:
    case (UI::SectionId)0x77:
        state = "Playing in an online battle!";
        break;
    case (UI::SectionId)0x6E:
    case (UI::SectionId)0x6F:
        state = "Spectating an online battle!";
        break;
    case (UI::SectionId)0x78:
    case (UI::SectionId)0x79:
        state = "Disconnected from online race!";
        break;
    case UI::SectionId::OnlineServer:
        state = "Hosting a room...";
        details = "Server Mode";
        break;
    case (UI::SectionId)0x94:
        state = "Configuring mission mode...";
        break;
    default:
        switch (sceneId) {
        case RKSceneID::Race:
            state = "In an unknown race!";
            break;
        case RKSceneID::Menu:
        case RKSceneID::Globe:
            state = "In an unknown menu!";
            break;
        default:
            break;
        }
        break;
    }

    if (sceneId == RKSceneID::Race) {
        auto timestamp = SP::IOSDolphin::GetSystemTime();
        startTimestamp = timestamp ? *timestamp : 0;
    }

    setGameState(state, details, startTimestamp);
}

RichPresenceManager::PresenceData &RichPresenceManager::status() {
    return m_cached;
}

bool RichPresenceManager::sendStatus() {
    if (!SP::IOSDolphin::Open()) {
        return false;
    }
    const bool result = SP::IOSDolphin::DiscordSetPresence(m_cached) == IPC_OK;
    m_statusWasSent |= result;
    return result;
}

void RichPresenceManager::setGameState(std::string_view state, std::string_view details,
        s64 startTimestamp) {
    if (!m_statusWasSent || state != std::string_view(status().state) ||
            startTimestamp != status().startTimestamp) {
        // TODO: replace m_state and details with BMG strings
        status().state = state;
        status().details = details;
        status().startTimestamp = startTimestamp;
        sendStatus();
    }
}

} // namespace System
