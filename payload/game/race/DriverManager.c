#include "DriverManager.h"

#include "../system/RaceConfig.h"

DriverManager *DriverManager_ct(DriverManager *this);

static DriverManager *my_DriverManager_createInstance(void) {
    s_driverManager = new (sizeof(DriverManager));
    DriverManager_ct(s_driverManager);

    if (s_raceConfig->raceScenario.players[0].type == PLAYER_TYPE_GHOST) {
        s_driverManager->liveViewWatchedPlayerId = 0;
    }

    return s_driverManager;
}
PATCH_B(DriverManager_createInstance, my_DriverManager_createInstance);
