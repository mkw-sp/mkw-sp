#include "ObjPylon01.h"

#include "../system/RaceConfig.h"

u32 my_ObjPylon01_vf_b0(const ObjPylon01 *this) {
    if (s_raceConfig->raceScenario.players[0].type != PLAYER_TYPE_GHOST && this->playerId != 0) {
        return 1; // Transparent
    }

    return 12; // Opaque
}
PATCH_B(ObjPylon01_vf_b0, my_ObjPylon01_vf_b0);
