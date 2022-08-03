#pragma once

extern "C" {
#include "SystemManager.h"
}

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

} // namespace System
