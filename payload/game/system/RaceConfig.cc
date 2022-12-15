#include "RaceConfig.hh"

extern "C" {
#include "game/system/SaveManager.h"
}
#include "game/system/SaveManager.hh"

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

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

void RaceConfig::applyVSEngineClass() {
    m_menuScenario.engineClass = EngineClass::CC150;
    m_menuScenario.mirror = false;
    m_menuScenario.mirrorRng = false;
    vsSpeedModIsEnabled = false;

    auto *saveManager = SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSClass>();
    switch (setting) {
    case SP::ClientSettings::VSClass::Mixed:
        m_menuScenario.mirror = hydro_random_uniform(20) >= 17;
        m_menuScenario.mirrorRng = true;
        break;
    case SP::ClientSettings::VSClass::CC150:
        break;
    case SP::ClientSettings::VSClass::Mirror:
        m_menuScenario.mirror = true;
        break;
    case SP::ClientSettings::VSClass::CC200:
        vsSpeedModIsEnabled = true;
        break;
    }
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

bool RaceConfig::Scenario::isSpOnline() const {
    switch (gameMode) {
    case GameMode::OnlineClient:
    case GameMode::OnlineServer:
        return true;
    default:
        return false;
    }
}

} // namespace System

extern "C" {

bool RaceConfig_IsSameTeam(u32 p0, u32 p1) {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    if (raceScenario.spMaxTeamSize < 2) {
        return p0 == p1;
    }

    return raceScenario.players[p0].spTeam == raceScenario.players[p1].spTeam;
}

}
