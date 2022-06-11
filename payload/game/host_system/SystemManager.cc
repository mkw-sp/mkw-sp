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
    OSRestart(OS_RESET_RESTART);
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
