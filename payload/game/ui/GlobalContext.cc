#include "GlobalContext.hh"

#include "CourseSelectPage.hh"

#include "game/system/SaveManager.hh"

namespace UI {

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

    CourseSelectPage::s_lastSelected = 0;
}

} // namespace UI
