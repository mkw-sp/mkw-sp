#include "Driver.hh"

#include "game/system/RaceConfig.hh"

namespace Race {

void Driver::onIndirectHit(u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(getPlayerId(), receiver)) {
        return;
    }

    REPLACED(onIndirectHit)(receiver);
}

} // namespace Race
