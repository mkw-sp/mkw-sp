#include "SectionManager.hh"

#include "game/host_system/SystemManager.hh"
#include "game/system/SaveManager.hh"

namespace UI {

Section *SectionManager::currentSection() {
    return m_currentSection;
}

SectionId SectionManager::nextSectionId() const {
    return m_nextSectionId;
}

SectionId SectionManager::lastSectionId() const {
    return m_lastSectionId;
}

RegisteredPadManager &SectionManager::registeredPadManager() {
    return m_registeredPadManager;
}

SaveManagerProxy *SectionManager::saveManagerProxy() {
    return m_saveManagerProxy;
}

GlobalContext *SectionManager::globalContext() {
    return m_globalContext;
}

void SectionManager::createSection() {
    REPLACED(createSection)();

    System::RichPresenceManager::Instance().onSectionChange(m_currentSection->id());
}

void SectionManager::destroySection() {
    m_saveManagerProxy->REPLACED(markLicensesDirty)();

    REPLACED(destroySection)();
}

void SectionManager::startChangeSection(s32 delay, u32 color) {
    if (color == 0xFF) {
        auto *saveManager = System::SaveManager::Instance();
        color = saveManager->getSetting<SP::ClientSettings::Setting::LoadingScreenColor>();
    }

    REPLACED(startChangeSection)(delay, color);
}

SectionManager *SectionManager::Instance() {
    return s_instance;
}

} // namespace UI
