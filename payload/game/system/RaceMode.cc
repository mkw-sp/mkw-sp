#include "RaceMode.hh"

#include "RaceConfig.hh"

namespace System {

bool TimeAttackRaceMode::canEndRace() {
    auto &raceScenario = RaceConfig::Instance()->raceScenario();
    for (u32 i = 0; i < raceScenario.playerCount; i++) {
        if (!m_raceManager->player(i)->hasFinished()) {
            switch (raceScenario.players[i].type) {
            case RaceConfig::Player::Type::Local:
                return false;
            case RaceConfig::Player::Type::Ghost:
                if (raceScenario.players[0].type != RaceConfig::Player::Type::Ghost) {
                    continue;
                }

                return false;
            default:
                continue;
            }
        }
    }

    return true;
}

} // namespace System
