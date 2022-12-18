#include "RaceManager.h"

#include "RaceConfig.h"

static bool my_TimeAttackGameMode_canEndRace(TimeAttackGameMode *UNUSED(this)) {
    const RaceConfigScenario *raceScenario = &s_raceConfig->raceScenario;
    for (u32 i = 0; i < raceScenario->playerCount; i++) {
        if (!s_raceManager->players[i]->hasFinished) {
            switch (raceScenario->players[i].type) {
            case PLAYER_TYPE_LOCAL:
                return false;
            case PLAYER_TYPE_GHOST:
                if (raceScenario->players[0].type != PLAYER_TYPE_GHOST) {
                    continue;
                }
                return false;
            }
        }
    }

    return true;
}
PATCH_B(TimeAttackGameMode_canEndRace, my_TimeAttackGameMode_canEndRace);
