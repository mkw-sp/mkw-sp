#include "RaceConfig.hh"
#include "SaveManager.hh"
#include "sp/settings/ClientSettings.hh"

namespace System {

RaceConfig::Scenario &RaceConfig::raceScenario() {
    return m_raceScenario;
}

RaceConfig::Scenario &RaceConfig::menuScenario() {
    return m_menuScenario;
}

RaceConfig::Scenario &RaceConfig::awardsScenario() {
    return m_awardsScenario;
}

u8 (&RaceConfig::ghostBuffers())[2][11][0x2800] {
    return m_ghostBuffers;
}
  
void RaceConfig::initRace() {
    REPLACED(initRace)();
    
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::TAMirror>();
    if (m_raceScenario.gameMode == GameMode::TimeAttack && setting == SP::ClientSettings::TAMirror::Enable) {
        m_raceScenario.mirror = true;
    }
    else {
        m_raceScenario.mirror = false;
    }
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

} // namespace System
