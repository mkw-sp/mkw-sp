#include "SystemManager.hh"

extern "C" {
#include <sp/IOSDolphin.h>

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
    IOSDolphin dolphin = IOSDolphin_Open();
    if (IOSDolphin_IsOpen(dolphin)) {
        IOSDolphin_SetSpeedLimit(dolphin, 100);
        IOSDolphin_Close(dolphin);
    }
}

void SystemManager::LaunchTitle(u64 titleID) {
    if (!OSIsTitleInstalled(titleID)) {
        return;
    }
    __OSLaunchTitle(titleID);
}

} // namespace System
