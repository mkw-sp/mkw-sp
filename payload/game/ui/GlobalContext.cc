#include "GlobalContext.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <sp/CourseDatabase.hh>

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

    SP::CourseDatabase::Instance().resetSelection();
}

void GlobalContext::clearCourses() {
    m_courseOrder.reset();
}

void GlobalContext::setCurrentCourse(Registry::Course course) {
    assert(m_courseOrder.count() == m_match);
    m_courseOrder.push_back(std::move(course));
}

std::optional<Registry::Course> GlobalContext::getCourse(u32 courseIdx) const {
    if (courseIdx >= m_courseOrder.count()) {
        return std::nullopt;
    } else {
        return *m_courseOrder[courseIdx];
    }
}

bool GlobalContext::generateRandomCourses() {
    if (getCourseSelection() != SP::ClientSettings::CourseSelection::Random) {
        return false;
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    SP::CourseDatabase::Filter filter = {
            .race = menuScenario.isVs(),
            .battle = menuScenario.isBattle(),
    };

    auto &courseDatabase = SP::CourseDatabase::Instance();
    auto courseCount = courseDatabase.count(filter);

    for (u8 i = 0; i < m_matchCount; i += 1) {
        Registry::Course randCourse;

        do {
            auto courseIdx = hydro_random_uniform(courseCount);
            randCourse = courseDatabase.entry(filter, courseIdx).courseId;
        } while (m_matchCount <= courseCount && m_courseOrder.contains(randCourse));

        m_courseOrder.push_back(std::move(randCourse));
    }

    menuScenario.courseId = getCourse(0).value();
    return true;
}

bool GlobalContext::generateOrderedCourses(u16 startingIndex) {
    if (getCourseSelection() != SP::ClientSettings::CourseSelection::InOrder) {
        return false;
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    SP::CourseDatabase::Filter filter = {
            .race = menuScenario.isVs(),
            .battle = menuScenario.isBattle(),
    };

    auto &courseDatabase = SP::CourseDatabase::Instance();
    auto courseCount = courseDatabase.count(filter);

    for (u8 i = 0; i < m_matchCount; i += 1) {
        auto courseIndex = (startingIndex + i) % courseCount;
        auto entry = courseDatabase.entry(filter, courseIndex);

        m_courseOrder.push_back(std::move(entry.courseId));
    }

    menuScenario.courseId = getCourse(0).value();
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

} // namespace UI
