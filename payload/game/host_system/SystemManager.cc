#include "SystemManager.hh"

#include "game/host_system/Scene.hh"
#include "game/system/RaceManager.hh"
#include "game/ui/Section.hh"

extern "C" {
#include <revolution.h>
}

namespace System {

void SystemManager::init() {
    REPLACED(init)();

    m_launchType = 1;
}

void SystemManager::shutdownSystem() {
    ShutdownSystem();
}

void SystemManager::returnToMenu() {
    ReturnToMenu();
}

void SystemManager::restart() {
    Restart();
}

u32 SystemManager::aspectRatio() const {
    return m_aspectRatio;
}

u32 SystemManager::matchingArea() const {
    return m_matchingArea;
}

u32 SystemManager::launchType() const {
    return m_launchType;
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
    auto sceneId = UI::Section::GetSceneId(sectionId);

    switch (sectionId) {
    case UI::SectionId::Thumbnails:
        state = "Generating thumbnails...";
        break;
    case UI::SectionId::CreateSave:
        state = "Creating new save...";
        break;
    case UI::SectionId::CorruptSave:
    case UI::SectionId::CountryChanged:
    case UI::SectionId::CannotSave:
    case UI::SectionId::CannotAccessNand:
    case UI::SectionId::CannotAccessMiis:
        state = "Error on launch!";
        break;
    case UI::SectionId::GPDemo:
        state = "Starting Grand Prix...";
        break;
    case UI::SectionId::VSDemo:
        state = "Starting VS Race...";
        break;
    case UI::SectionId::BTDemo:
        state = "Starting Battle Mode...";
        break;
    case UI::SectionId::MRBossDemo:
        state = "Starting Mission Mode...";
        break;
    case UI::SectionId::CoBossDemo:
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
    case UI::SectionId::MR:
        state = "Playing a mission!";
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
    case UI::SectionId::Unknown37:
    case UI::SectionId::AwardsBT:
        state = "In an awards ceremony!";
        break;
    case UI::SectionId::CreditsP1:
    case UI::SectionId::CreditsP1True:
    case UI::SectionId::CreditsP2:
    case UI::SectionId::CreditsP2True:
    case UI::SectionId::Congratulations:
    case UI::SectionId::CongratulationsTrue:
        state = "Playing credits!";
        break;
    case UI::SectionId::TitleFromBoot:
    case UI::SectionId::TitleFromReset:
    case UI::SectionId::TitleFromMenu:
    case UI::SectionId::TitleFromNewLicence:
    case UI::SectionId::TitleFromOptions:
        state = "Selecting license/game type...";
        break;
    case UI::SectionId::Demo:
        state = "Watching the intro movie!";
        break;
    case UI::SectionId::MiiSelectCreate:
    case UI::SectionId::MiiSelectChange:
    case UI::SectionId::LicenseSettings:
        state = "Changing license settings...";
        break;
    case UI::SectionId::Single:
    case UI::SectionId::SingleChangeDriver:
    case UI::SectionId::SingleChangeCourse:
    case UI::SectionId::SingleSelectVSCourse:
    case UI::SectionId::SingleSelectBTCourse:
    case UI::SectionId::SingleChangeMission:
    case UI::SectionId::SingleChangeGhostData:
        state = "Configuring singleplayer race...";
        break;
    case UI::SectionId::Multi:
        state = "Configuring multiplayer race...";
        break;
    case UI::SectionId::OnlineSingle:
    case UI::SectionId::OnlineMultiConfigure:
    case UI::SectionId::OnlineMulti:
        state = "Searching for a room...";
        break;
    case UI::SectionId::WifiSingleDisconnected:
    case UI::SectionId::WifiMultiDisconnected:
        state = "Disconnected from the server!";
        break;
    case UI::SectionId::WifiSingleFriendList:
        state = "Viewing friend list...";
        break;
    case UI::SectionId::WifiSingleVsVoting:
    case UI::SectionId::WifiSingleBtVoting:
    case UI::SectionId::WifiMultiVsVoting:
    case UI::SectionId::WifiMultiBtVoting:
    case UI::SectionId::Voting1PVS:
    case UI::SectionId::Voting1PBalloon:
    case UI::SectionId::Voting1PCoin:
    case UI::SectionId::Voting2PVS:
    case UI::SectionId::Voting2PBalloon:
    case UI::SectionId::Voting2PCoin:
        state = "Voting for online track!";
        break;
    case UI::SectionId::WifiVS:
    case UI::SectionId::WifiMultiVS:
    case UI::SectionId::OnlineFriend1PVS:
    case UI::SectionId::OnlineFriend2PVS:
        state = "Playing in an online VS race!";
        break;
    case UI::SectionId::WifiVSSpectate:
    case UI::SectionId::WifiVSMultiSpectate:
        state = "Spectating an online race!";
        break;
    case UI::SectionId::WifiBT:
    case UI::SectionId::WifiMultiBT:
    case UI::SectionId::OnlineFriend1PBalloon:
    case UI::SectionId::OnlineFriend1PCoin:
    case UI::SectionId::OnlineFriend2PBalloon:
    case UI::SectionId::OnlineFriend2PCoin:
        state = "Playing in an online battle!";
        break;
    case UI::SectionId::WifiBTSpectate:
    case UI::SectionId::WifiBTMultiSpectate:
        state = "Spectating an online battle!";
        break;
    case UI::SectionId::OnlineDisconnected:
    case UI::SectionId::OnlineDisconnectedGeneric:
        state = "Disconnected from online race!";
        break;
    case UI::SectionId::MissionMenu:
        state = "Configuring mission mode...";
        break;
    default:
        switch (sceneId) {
        case SceneId::Race:
            state = "In an unknown race!";
            break;
        case SceneId::Menu:
        case SceneId::Globe:
            state = "In an unknown menu!";
            break;
        default:
            break;
        }
        break;
    }

    if (SP::IOSDolphin::Open() && (sceneId == SceneId::Race)) {
        auto timestamp = SP::IOSDolphin::GetSystemTime();
        startTimestamp = timestamp ? *timestamp / 1000 : 0;
    }

    // NOTE (vabold): Two states can have the same detail, but two details cannot have the same
    // state.
    assert(!(state == std::string_view(status().state) &&
            details != std::string_view(status().details)));

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
            details != std::string_view(status().details) ||
            startTimestamp != status().startTimestamp) {
        // TODO: replace m_state and details with BMG strings
        status().state = state;
        status().details = details;
        status().startTimestamp = startTimestamp;
        m_statusWasSent = false;
        sendStatus();
    }
}

} // namespace System
