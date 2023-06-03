extern "C" {
#include "ObjPylon01.h"
}

#include "game/system/RaceConfig.hh"

u32 ObjPylon01_vf_b0(const ObjPylon01 *self) {
    auto *cfg = System::RaceConfig::Instance();
    if (cfg->raceScenario().players[0].type != System::RaceConfig::Player::Type::Ghost &&
            self->playerId != 0) {
        return 1; // Transparent
    }

    return 12; // Opaque
}
