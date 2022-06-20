#include "SystemManager.hh"

extern "C" {
#include <sp/IOSDolphin.h>

#include <revolution.h>
}

namespace System {

static void ResetDolphinSpeedLimit() {
    IOSDolphin dolphin = IOSDolphin_Open();
    if (IOSDolphin_IsOpen(dolphin)) {
        IOSDolphin_SetSpeedLimit(dolphin, 100);
        IOSDolphin_Close(dolphin);
    }
}

static void LaunchTitle(u64 titleID) {
    if (!OSIsTitleInstalled(titleID)) {
        return;
    }
    __OSLaunchTitle(titleID);
}

void SystemManager::shutdownSystem() {
    ResetDolphinSpeedLimit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    OSShutdownSystem();
}

void SystemManager::returnToMenu() {
    ResetDolphinSpeedLimit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    VIWaitForRetrace();
    OSReturnToMenu();
}

void SystemManager::restart() {
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

} // namespace System

extern "C" void my_SystemManager_shutdownSystem(SystemManager *self) {
    reinterpret_cast<System::SystemManager *>(self)->shutdownSystem();
}

extern "C" void my_SystemManager_returnToMenu(SystemManager *self) {
    reinterpret_cast<System::SystemManager *>(self)->returnToMenu();
}

extern "C" void my_SystemManager_restart(SystemManager *self) {
    reinterpret_cast<System::SystemManager *>(self)->restart();
}
