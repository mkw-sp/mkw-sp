#include "SectionManager.hh"

#include "game/system/SaveManager.hh"

namespace UI {

Section *SectionManager::currentSection() {
    return m_currentSection;
}

SaveManagerProxy *SectionManager::saveManagerProxy() {
    return m_saveManagerProxy;
}

GlobalContext *SectionManager::globalContext() {
    return m_globalContext;
}

void SectionManager::startChangeSection(s32 delay, u32 color) {
    if (color == 0xFF) {
        auto *saveManager = System::SaveManager::Instance();
        color = saveManager->getSetting<SP::ClientSettings::Setting::LoadingScreenColor>();
    }

    REPLACED(startChangeSection)(delay, color);
}

bool SectionManager::taIsVanilla() const {
    return m_currentSection->id() == SectionId::TA && m_globalContext->m_timeAttackIsVanilla;
}

SectionManager *SectionManager::Instance() {
    return s_instance;
}

} // namespace UI
