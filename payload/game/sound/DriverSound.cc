#include "DriverSound.hh"

#include "game/system/RaceConfig.hh"

namespace Sound {

void DriverSound::onIndirectHit(u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(m_playerId, receiver)) {
        return;
    }

    REPLACED(onIndirectHit)(receiver);
}

} // namespace Sound
