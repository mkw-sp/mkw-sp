#include "GlobalContext.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/CourseSelectPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/trackPacks/TrackPackManager.hh>

// Imported by SectionManager.S
extern "C" const s16 GlobalContext_SizeInBytes;
const s16 GlobalContext_SizeInBytes = sizeof(UI::GlobalContext);

namespace UI {

SP::ClientSettings::CourseSelection getCourseSelection() {
    auto *saveManager = System::SaveManager::Instance();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();

    using GameMode = System::RaceConfig::GameMode;
    if (menuScenario.gameMode == GameMode::TimeAttack) {
        return SP::ClientSettings::CourseSelection::Choose;
    } else if (menuScenario.gameMode == GameMode::OfflineVS) {
        return saveManager->getSetting<SP::ClientSettings::Setting::VSCourseSelection>();
    } else if (menuScenario.gameMode == GameMode::OfflineBT) {
        return saveManager->getSetting<SP::ClientSettings::Setting::BTCourseSelection>();
    } else {
        panic("Unknown gamemode %d", static_cast<u32>(menuScenario.gameMode));
    }
}

void GlobalContext::reset() {
    REPLACED(reset)();
    m_courseOrder.reset();
}

void GlobalContext::onChangeLicense() {
    REPLACED(onChangeLicense)();

    auto *saveManager = System::SaveManager::Instance();
    Registry::Character character =
            saveManager->getSetting<SP::ClientSettings::Setting::Character>();
    Registry::Vehicle vehicle = saveManager->getSetting<SP::ClientSettings::Setting::Vehicle>();
    if (!Registry::IsCombinationValid(character, vehicle)) {
        character = Registry::Character::Mario;
        vehicle = Registry::Vehicle::StandardKartMedium;
    }
    m_localCharacterIds[0] = character;
    m_localVehicleIds[0] = vehicle;

    for (size_t i = 0; i < std::size(m_driftModes); i++) {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DriftMode>();
        m_driftModes[i] = static_cast<u32>(setting) + 1;
    }

    UI::CourseSelectPage::s_lastSelected.reset();
}

void GlobalContext::clearCourses() {
    m_courseOrder.reset();
}

bool GlobalContext::isVanillaTracks() const {
    return m_currentPack == 0;
}

void GlobalContext::setCurrentTrack(SP::Track track) {
    assert(m_courseOrder.count() == m_match);
    m_courseOrder.push_back(std::move(track));
}

const SP::Track *GlobalContext::getTrack(u32 courseIdx) const {
    if (courseIdx >= m_courseOrder.count()) {
        return nullptr;
    } else {
        return m_courseOrder[courseIdx];
    }
}

bool GlobalContext::generateRandomCourses() {
    if (getCourseSelection() != SP::ClientSettings::CourseSelection::Random) {
        return false;
    }

    auto *raceConfig = System::RaceConfig::Instance();
    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &pack = trackPackManager.getSelectedPack();

    SP::Track::Mode filter = raceConfig->menuScenario().getTrackMode();
    auto trackCount = pack.getTrackCount(filter);

    for (u8 i = 0; i < m_matchCount; i += 1) {
        Sha1 randCourseId;

        do {
            auto courseIdx = hydro_random_uniform(trackCount);
            randCourseId = *pack.getNthTrack(courseIdx, filter);
        } while (m_matchCount <= trackCount && inCourseQueue(randCourseId));

        auto randCourse = trackPackManager.getTrack(randCourseId);
        m_courseOrder.push_back(std::move(randCourse));
    }

    getTrack(0)->applyToConfig(raceConfig, false);
    return true;
}

bool GlobalContext::generateOrderedCourses(u16 startingIndex) {
    if (getCourseSelection() != SP::ClientSettings::CourseSelection::InOrder) {
        return false;
    }

    auto *raceConfig = System::RaceConfig::Instance();
    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &pack = trackPackManager.getSelectedPack();

    SP::Track::Mode filter = raceConfig->menuScenario().getTrackMode();
    auto trackCount = pack.getTrackCount(filter);

    for (u8 i = 0; i < m_matchCount; i += 1) {
        auto courseIndex = (startingIndex + i) % trackCount;
        auto courseId = pack.getNthTrack(courseIndex, filter);
        auto &course = trackPackManager.getTrack(*courseId);

        m_courseOrder.push_back(std::move(course));
    }

    getTrack(0)->applyToConfig(raceConfig, false);
    return true;
}

void GlobalContext::applyVehicleRestriction(bool isBattle) {
    auto *saveManager = System::SaveManager::Instance();

    SP::ClientSettings::Vehicles setting;
    if (isBattle) {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::BTVehicles>();
    } else {
        setting = saveManager->getSetting<SP::ClientSettings::Setting::VSVehicles>();
    }

    switch (setting) {
    case SP::ClientSettings::Vehicles::All:
        m_vehicleRestriction = VehicleRestriction::All;
        break;
    case SP::ClientSettings::Vehicles::Karts:
        m_vehicleRestriction = VehicleRestriction::KartsOnly;
        break;
    case SP::ClientSettings::Vehicles::Bikes:
        m_vehicleRestriction = VehicleRestriction::BikesOnly;
        break;
    }
}

bool GlobalContext::inCourseQueue(Sha1 trackSha) const {
    for (u32 i = 0; i < m_courseOrder.count(); i += 1) {
        if (m_courseOrder[i]->m_sha1 == trackSha) {
            return true;
        }
    }

    return false;
}

} // namespace UI
