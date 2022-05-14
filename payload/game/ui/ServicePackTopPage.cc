#include "ServicePackTopPage.hh"

namespace UI {

ServicePackTopPage::ServicePackTopPage() : m_onBack(this, &UI::ServicePackTopPage::onBack) {}

void ServicePackTopPage::dt(s32 type) {
    m_backButton.dt(-1);
    m_aboutButton.dt(-1);
    m_channelButton.dt(-1);
    m_updatesButton.dt(-1);
    m_ghostsButton.dt(-1);
    m_tracksButton.dt(-1);
    m_settingsButton.dt(-1);
    m_pageTitleText.dt(-1);
    m_inputManager.dt(-1);

    Page::dt(0);
    if (type > 0) {
        delete this;
    }
}

void ServicePackTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(8);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_tracksButton, 0);
    insertChild(3, &m_ghostsButton, 0);
    insertChild(4, &m_updatesButton, 0);
    insertChild(5, &m_channelButton, 0);
    insertChild(6, &m_aboutButton, 0);
    insertChild(7, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "ServicePackTopButton", "Settings", 0x1, false, false);
    m_tracksButton.load("button", "ServicePackTopButton", "Tracks", 0x1, false, false);
    m_ghostsButton.load("button", "ServicePackTopButton", "Ghosts", 0x1, false, false);
    m_updatesButton.load("button", "ServicePackTopButton", "Updates", 0x1, false, false);
    m_channelButton.load("button", "ServicePackTopButton", "Channel", 0x1, false, false);
    m_aboutButton.load("button", "ServicePackTopButton", "About", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    m_pageTitleText.setMessage(10083, nullptr);

    m_settingsButton.selectDefault(0);
}

void ServicePackTopPage::onBack(u32 UNUSED(localPlayerId)) {
    changeSection(SectionId::TitleFromOptions, Animation::Prev, 0.0f);
}

} // namespace UI

extern "C" void _ZN2UI18ServicePackTopPageC1Ev(void *self);
extern "C" void *ServicePackTopPage_ct(void *self) {
    _ZN2UI18ServicePackTopPageC1Ev(self);
    return self;
}

static_assert(sizeof_ServicePackTopPage == sizeof(UI::ServicePackTopPage));
