#include "RaceConfig.hh"

#include "game/system/InputManager.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include "game/system/SaveManager.h"
}

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

#include <sp/settings/ClientSettings.hh>

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

void RaceConfig::applyEngineClass() {
    if (m_menuScenario.gameMode != RaceConfig::GameMode::OfflineVS) {
        SP_LOG("applyEngineClass called with invalid GameMode");
        assert(false);
    }

    auto setting = SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::VSClass>();

    m_menuScenario.engineClass = EngineClass::CC150;
    m_menuScenario.mirrorRng = false;
    m_menuScenario.mirror = false;

    switch (setting) {
    case SP::ClientSettings::EngineClass::Mixed:
        m_menuScenario.mirror = hydro_random_uniform(20) >= 17;
        m_menuScenario.mirrorRng = true;
        break;
    case SP::ClientSettings::EngineClass::CC50:
        m_menuScenario.engineClass = EngineClass::CC50;
        break;
    case SP::ClientSettings::EngineClass::CC100:
        m_menuScenario.engineClass = EngineClass::CC100;
        break;
    case SP::ClientSettings::EngineClass::CC150: // Set above
    case SP::ClientSettings::EngineClass::CC200: // handled in KartObjectManager
        break;
    case SP::ClientSettings::EngineClass::Mirror:
        m_menuScenario.mirror = true;
    }
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

void RaceConfig::ConfigurePlayers(Scenario &scenario, u32 screenCount) {
    auto *inputManager = InputManager::Instance();

    u8 screenId = 0, ghostProxyId = 0;

    for (u32 i = 0; i < 12; i++) {
        if (scenario.players[i].type == Player::Type::Local) {
            assert(ghostProxyId < 4);

            s32 controllerId = inputManager->ghostProxy(ghostProxyId)->pad()->getControllerId();
            scenario.players[i].screenId = screenId;
            scenario.players[i].ghostProxyId = ghostProxyId;
            scenario.players[i].controllerId = controllerId;
            scenario.screenPlayerIds[screenId] = i;

            screenId++;
            ghostProxyId++;
        } else if (scenario.players[i].type == Player::Type::Ghost) {
            u32 ghostId = scenario.players[0].type == Player::Type::Ghost ? i : i - 1;
            const u8 *rawGhostFile = (*scenario.ghostBuffer)[ghostId];

            if (RawGhostFile::IsValid(rawGhostFile)) {
                auto *rawGhostHeader = reinterpret_cast<const RawGhostHeader *>(rawGhostFile);
                bool driftIsAuto = rawGhostHeader->driftIsAuto;
                inputManager->setGhostPad(i, rawGhostFile + 0x88, driftIsAuto);
                scenario.players[i].characterId = rawGhostHeader->characterId;
                scenario.players[i].vehicleId = rawGhostHeader->vehicleId;
                scenario.players[i].controllerId = rawGhostHeader->controllerId;
            } else {
                inputManager->setGhostPad(i, rawGhostFile + 0x88, false);
                scenario.players[i].characterId = 0;  // Mario
                scenario.players[i].vehicleId = 1;    // Standard Kart M
                scenario.players[i].controllerId = 0; // Wii Wheel
            }
        } else {
            scenario.players[i].controllerId = -1;
        }
    }

    for (u32 i = 0; i < 12 && screenId < screenCount; i++) {
        if (scenario.players[i].type == Player::Type::None) {
            continue;
        }

        if (scenario.players[i].screenId != -1) {
            continue;
        }

        scenario.players[i].screenId = screenId;
        scenario.screenPlayerIds[screenId] = i;

        screenId++;
    }
}

void RaceConfig::initRace() {
    REPLACED(initRace)();
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::TAMirror>();
    // Switch the race to mirror if the mirror TT setting is enabled.
    if (m_raceScenario.gameMode == GameMode::TimeAttack &&
            setting == SP::ClientSettings::TAMirror::Enable) {
        m_raceScenario.mirror = true;
    } else if (m_raceScenario.gameMode == GameMode::TimeAttack &&
            setting == SP::ClientSettings::TAMirror::Disable) {
        m_raceScenario.mirror = false;
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
