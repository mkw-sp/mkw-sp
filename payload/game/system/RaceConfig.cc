#include "RaceConfig.hh"

#include "game/system/InputManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <tuple>
extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

namespace System {

void RaceConfig::nextCourseIndex() {
    m_currentCourse += 1;
}

bool RaceConfig::outOfTracks() const {
    return m_currentCourse >= m_courseOrder.count();
}

u32 RaceConfig::getCourseIndex() const {
    return m_currentCourse;
}

bool RaceConfig::isVanillaTracks() const {
    return m_selectedTrackPack == 0;
}

SP::TrackPackInfo &RaceConfig::getPackInfo() {
    assert(!outOfTracks());
    return *m_courseOrder[m_currentCourse];
}

SP::TrackPackInfo &RaceConfig::emplacePackInfo() {
    assert(m_courseOrder.count() == m_currentCourse);

    SP::TrackPackInfo packInfo = {};
    m_courseOrder.push_back(std::move(packInfo));
    return *m_courseOrder[m_currentCourse];
}

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

    if (setting != SP::ClientSettings::CPUMode::None) {
        m_menuScenario.cpuMode = static_cast<u32>(setting);
        return;
    }

    for (u32 i = 1; i < 12; i++) {
        if (m_menuScenario.players[i].type == Player::Type::CPU) {
            m_menuScenario.players[i].type = Player::Type::None;
        }
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

void RaceConfig::init() {
    REPLACED(init)();
    m_courseOrder = {};
    m_currentCourse = 0;
    m_selectedTrackPack = 0;
}

void RaceConfig::clear() {
    REPLACED(clear)();
    m_courseOrder.reset();
    m_currentCourse = 0;
    m_selectedTrackPack = 0;
}

void RaceConfig::initRace() {
    REPLACED(initRace)();

    // Setup stock game slots
    if (m_menuScenario.gameMode != GameMode::Awards) {
        m_raceScenario.courseId = getPackInfo().getSelectedCourse();
    }
}

std::tuple<SP::TrackGameMode, SP::ClientSettings::CourseSelection> RaceConfig::getCourseSettings() {
    auto *saveManager = SaveManager::Instance();
    if (m_menuScenario.gameMode == GameMode::TimeAttack) {
        return std::make_tuple(SP::TrackGameMode::Race,
                SP::ClientSettings::CourseSelection::Choose);
    } else if (m_menuScenario.gameMode == GameMode::OfflineVS) {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSCourseSelection>();
        return std::make_tuple(SP::TrackGameMode::Race, setting);
    } else if (m_menuScenario.gameMode == GameMode::OfflineBT) {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::BTCourseSelection>();

        if (m_menuScenario.battleType == 0) {
            return std::make_tuple(SP::TrackGameMode::Balloon, setting);
        } else {
            return std::make_tuple(SP::TrackGameMode::Coin, setting);
        }
    } else {
        panic("Unknown gamemode %d", static_cast<u32>(m_menuScenario.gameMode));
    }
}

bool RaceConfig::generateRandomCourses() {
    auto [mode, courseSelection] = getCourseSettings();
    if (courseSelection != SP::ClientSettings::CourseSelection::Random) {
        return false;
    }

    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &trackPack = trackPackManager.getSelectedTrackPack();
    auto raceCount = UI::SectionManager::Instance()->globalContext()->m_matchCount;

    for (u8 i = 0; i < raceCount; i += 1) {
        auto courseCount = trackPack.getTrackCount(mode);
        auto courseIdx = hydro_random_uniform(courseCount);
        auto randCourse = trackPack.getNthTrack(courseIdx, mode);

        SP::TrackPackInfo packInfo = {};
        packInfo.selectCourse(randCourse.value());
        m_courseOrder.push_back(std::move(packInfo));
    }

    return true;
}

bool RaceConfig::generateOrderedCourses(u16 currentIdx) {
    auto [mode, courseSelection] = getCourseSettings();
    if (courseSelection != SP::ClientSettings::CourseSelection::InOrder) {
        return false;
    }

    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &trackPack = trackPackManager.getSelectedTrackPack();
    auto raceCount = UI::SectionManager::Instance()->globalContext()->m_matchCount;

    for (u8 i = 0; i < raceCount; i += 1) {
        auto track = trackPack.getNthTrack(currentIdx, mode);
        if (track.has_value()) {
            currentIdx += 1;
        } else {
            track = trackPack.getNthTrack(0, mode);
            currentIdx = 1;
        }

        SP::TrackPackInfo packInfo = {};
        packInfo.selectCourse(*track);
        m_courseOrder.push_back(std::move(packInfo));
    }

    return true;
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
