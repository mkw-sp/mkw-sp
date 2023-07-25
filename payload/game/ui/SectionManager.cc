#include "SectionManager.hh"

#include "game/host_system/Scene.hh"
#include "game/host_system/SystemManager.hh"
#include "game/system/SaveManager.hh"

#include <sp/WU8Library.hh>

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

void SectionManager::init() {
    REPLACED(init)();

    if (SP::WU8Library::ShouldExtract()) {
        m_nextSectionId = SectionId::WU8Library;
    }
}

void SectionManager::createSection() {
    if (m_firstLoad && System::SystemManager::Instance()->launchType() == 1) {
        m_registeredPadManager._8061B5A4();
    }
    auto nextId = m_nextSectionId;
    m_currentAnimDir = m_nextAnimDir;
    m_nextSectionId = UI::SectionId::None;
    m_changeTimer = 0;
    m_nextAnimDir = 0;
    m_transitionFrame = -1;
    m_state = 0;
    bool race = Section::GetSceneId(nextId) == System::SceneId::Race;
    m_registeredPadManager.onCreateSection(!race);
    m_currentSection = new Section;
    m_currentSection->init(nextId);
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

void SectionManager::transitionToError(u32 errorCode) {
    m_globalContext->m_onlineDisconnectInfo.m_category = UI::OnlineErrorCategory::ErrorCode;
    m_globalContext->m_onlineDisconnectInfo.m_errorCode = errorCode;

    setNextSection(UI::SectionId::OnlineDisconnected, UI::Page::Anim::None);
    startChangeSection(0, 0);
}

SectionManager *SectionManager::Instance() {
    return s_instance;
}

} // namespace UI
