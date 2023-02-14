#include "BalloonManager.hh"

#include "game/system/RaceConfig.hh"

namespace Battle {

void BalloonManager::add(u32 playerId, u32 /* teamId */, u32 r6, u32 r7, u32 count, u32 r9) {
    REPLACED(add)(playerId, 0, r6, r7, count, r9);
}

BalloonManager *BalloonManager::CreateInstance() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode == System::RaceConfig::GameMode::OfflineBT &&
            raceScenario.battleType == 0) {
        s_instance = new BalloonManager;
    }

    return s_instance;
}

BalloonManager::BalloonManager() {
    const auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    m_playerCount = raceScenario.playerCount;
    m_balloonsPerTeam = m_playerCount * 5;
    m_balloonCount = m_balloonsPerTeam;
    for (u8 i = 0; i < m_playerCount; i++) {
        m_players[i] = {};
    }
    for (u8 i = 0; i < m_balloonCount; i++) {
        m_balloons[i].unit = new BalloonUnit(i, 0);
        m_balloons[i].unit->init();
        m_balloons[i].teamId = 0;
    }
}

} // namespace Battle
