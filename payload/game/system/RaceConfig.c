#include "RaceConfig.h"

#include <string.h>

void RaceConfigScenario_resetGhostPlayerTypes(RaceConfigScenario *this) {
    for (u32 i = 1; i < 12; i++) {
        this->players[i].type = PLAYER_TYPE_NONE;
    }
}

static RaceConfig *my_RaceConfig_createInstance(void) {
    s_raceConfig = new (sizeof(RaceConfig));
    RaceConfig_ct(s_raceConfig);

    s_raceConfig->raceScenario.ghostBuffer = s_raceConfig->ghostBuffers + 0;
    s_raceConfig->menuScenario.ghostBuffer = s_raceConfig->ghostBuffers + 1;

    memset(s_raceConfig->ghostBuffers, 0, sizeof(s_raceConfig->ghostBuffers));

    return s_raceConfig;
}
PATCH_B(RaceConfig_createInstance, my_RaceConfig_createInstance);
