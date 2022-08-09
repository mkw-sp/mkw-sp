#include "SystemManager.hh"

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

RichPresenceManager& RichPresenceManager::Instance() {
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
        SP_LOG("Discord RPC could not be enabled because Config::MAIN_USE_DISCORD_PRESENCE is not set");
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

void SystemManager::LaunchTitle(u64 titleID) {
    if (!OSIsTitleInstalled(titleID)) {
        return;
    }
    __OSLaunchTitle(titleID);
}

}  // namespace System
