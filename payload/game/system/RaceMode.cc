#include "RaceMode.hh"

#include "game/system/RaceConfig.hh"

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

void BalloonBattleRaceMode::onRemoveHit(u32 sender, u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(sender, receiver)) {
        onRemoveHit(receiver);
        return;
    }

    REPLACED(onRemoveHit)(sender, receiver);
}

void BalloonBattleRaceMode::onMoveHit(u32 sender, u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(sender, receiver)) {
        return;
    }

    REPLACED(onMoveHit)(sender, receiver);
}

void CoinBattleRaceMode::onRemoveHit(u32 sender, u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(sender, receiver)) {
        onRemoveHit(receiver);
        return;
    }

    REPLACED(onRemoveHit)(sender, receiver);
}

void CoinBattleRaceMode::onMoveHit(u32 sender, u32 receiver) {
    if (System::RaceConfig::Instance()->isSameTeam(sender, receiver)) {
        return;
    }

    REPLACED(onMoveHit)(sender, receiver);
}

} // namespace System
