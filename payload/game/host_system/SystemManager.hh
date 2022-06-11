#pragma once

extern "C" {
#include "SystemManager.h"
}

namespace System {

class SystemManager {
public:
    void shutdownSystem();
    void returnToMenu();
    void restart();

private:
    u8 _0000[0x1100 - 0x0000];
};
static_assert(sizeof(SystemManager) == 0x1100);

} // namespace System
