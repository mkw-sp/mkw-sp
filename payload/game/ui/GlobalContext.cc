#include "GlobalContext.hh"

#include "game/system/SaveManager.hh"

namespace UI {

void GlobalContext::onChangeLicense() {
    REPLACED(onChangeLicense)();

    auto *saveManager = System::SaveManager::Instance();
    for (size_t i = 0; i < std::size(m_driftModes); i++) {
        auto setting = saveManager->getSetting<SP::ClientSettings::Setting::DriftMode>();
        m_driftModes[i] = static_cast<u32>(setting) + 1;
    }
}

} // namespace UI
