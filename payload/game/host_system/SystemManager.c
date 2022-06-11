#include "SystemManager.h"

PATCH_B(SystemManager_shutdownSystem, my_SystemManager_shutdownSystem);
PATCH_B(SystemManager_returnToMenu, my_SystemManager_returnToMenu);
PATCH_B(SystemManager_restart, my_SystemManager_restart);
