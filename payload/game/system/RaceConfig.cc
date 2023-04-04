#include "RaceConfig.hh"

#include "game/system/InputManager.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

#include <cstring>

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

bool RaceConfig::isSameTeam(u32 p0, u32 p1) const {
    if (m_raceScenario.spMaxTeamSize < 2) {
        return p0 == p1;
    }

    return m_raceScenario.players[p0].spTeam == m_raceScenario.players[p1].spTeam;
}

void RaceConfig::applyPlayers() {
    auto *saveManager = System::SaveManager::Instance();

    u8 minPlayerCount = 1;
    if (m_menuScenario.gameMode == GameMode::OfflineVS) {
        minPlayerCount = saveManager->getSetting<SP::ClientSettings::Setting::VSPlayerCount>();
    } else if (m_menuScenario.gameMode == GameMode::OfflineBT) {
        minPlayerCount = saveManager->getSetting<SP::ClientSettings::Setting::BTPlayerCount>();
    }

    u8 i = 0;
    for (; i < m_menuScenario.localPlayerCount; i++) {
        m_menuScenario.players[i].type = Player::Type::Local;
    }
    for (; i < minPlayerCount; i++) {
        m_menuScenario.players[i].type = Player::Type::CPU;
    }
    for (; i < 12; i++) {
        m_menuScenario.players[i].type = Player::Type::None;
    }
}

void RaceConfig::applyEngineClass() {
    auto *saveManager = SaveManager::Instance();
    auto setting = SP::ClientSettings::EngineClass::CC150;

    if (m_menuScenario.gameMode == GameMode::OfflineVS) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::VSClass>();
    } else if (m_menuScenario.gameMode == GameMode::OfflineBT) {
        setting = SP::ClientSettings::EngineClass::CC50;
    } else if (m_menuScenario.gameMode == GameMode::TimeAttack) {
        auto taSetting = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
        if (taSetting == SP::ClientSettings::TAClass::CC200) {
            setting = SP::ClientSettings::EngineClass::CC200;
        } else if (taSetting == SP::ClientSettings::TAClass::Mirror) {
            setting = SP::ClientSettings::EngineClass::Mirror;
        }
    }

    m_menuScenario.engineClass = EngineClass::CC150;
    m_menuScenario.mirrorRng = false;
    m_menuScenario.is200cc = false;
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
        break;
    case SP::ClientSettings::EngineClass::CC200: // handled in KartObjectManager
        m_menuScenario.is200cc = true;
        break;
    case SP::ClientSettings::EngineClass::Mirror:
        m_menuScenario.mirror = true;
    }
}

void RaceConfig::applyItemFreq() {
    SP::ClientSettings::ItemFrequency setting;

    auto *saveManager = SaveManager::Instance();
    if (m_menuScenario.gameMode == GameMode::OfflineVS) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::VSItemFrequency>();
    } else if (m_menuScenario.gameMode == GameMode::OfflineBT) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::BTItemFrequency>();
    } else {
        panic("applyCPUMode called with invalid GameMode");
    }

    m_menuScenario.itemMode = static_cast<u32>(setting);
}

void RaceConfig::applyCPUMode() {
    SP::ClientSettings::CPUMode setting;

    auto *saveManager = SaveManager::Instance();
    if (m_menuScenario.gameMode == GameMode::OfflineVS) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::VSCPUMode>();
    } else if (m_menuScenario.gameMode == GameMode::OfflineBT) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::BTCPUMode>();
    } else {
        panic("applyCPUMode called with invalid GameMode");
    }

    m_menuScenario.cpuMode = static_cast<u32>(setting);
}

void RaceConfig::initRace() {
    REPLACED(initRace)();
    m_spRace = m_spMenu;
}

void RaceConfig::initAwards() {
    REPLACED(initAwards)();
    m_spRace = m_spMenu;
    m_spAwards = m_spRace;
}

void RaceConfig::initCredits() {
    REPLACED(initCredits)();
    m_spMenu = m_spAwards;
    m_spRace = m_spMenu;
}
RaceConfig *RaceConfig::CreateInstance() {
    assert(!s_instance);
    s_instance = new RaceConfig;

    s_instance->m_raceScenario.ghostBuffer = &s_instance->m_ghostBuffers[0];
    s_instance->m_menuScenario.ghostBuffer = &s_instance->m_ghostBuffers[1];

    memset(s_instance->m_ghostBuffers, 0, sizeof(s_instance->m_ghostBuffers));
    return s_instance;
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

            Registry::Controller controllerId =
                    inputManager->ghostProxy(ghostProxyId)->pad()->getControllerId();
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
                scenario.players[i].characterId = 0; // Mario
                scenario.players[i].vehicleId = 1;   // Standard Kart M
                scenario.players[i].controllerId = Registry::Controller::WiiWheel;
            }
        } else {
            scenario.players[i].controllerId = Registry::Controller::None;
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

extern "C" void RaceConfigScenario_resetGhostPlayerTypes(RaceConfig::Scenario *self) {
    for (u32 i = 1; i < 12; i++) {
        self->players[i].type = RaceConfig::Player::Type::None;
    }
}

} // namespace System
