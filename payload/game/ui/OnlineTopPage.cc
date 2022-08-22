#include "OnlineTopPage.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

namespace UI {

OnlineTopPage::OnlineTopPage() = default;

OnlineTopPage::~OnlineTopPage() = default;

PageId OnlineTopPage::getReplacement() {
    return m_replacement;
}

void OnlineTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(4);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_connectButton, 0);
    insertChild(3, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "SettingsButton", "Option", 0x1, false, false);
    m_connectButton.load("button", "OnlineTopButton", "Connect", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_connectButton.setFrontHandler(&m_onConnectButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    if (SectionManager::Instance()->currentSection()->id() == SectionId::OnlineSingle) {
        m_pageTitleText.setMessage(20003);
    } else {
        m_pageTitleText.setMessage(20004);
    }

    m_connectButton.selectDefault(0);
}

void OnlineTopPage::onActivate() {
    m_replacement = PageId::None;
}

void OnlineTopPage::onBack([[maybe_unused]] u32 localPlayerId) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
}

void OnlineTopPage::onSettingsButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, PageId::OnlineTop);
    m_replacement = PageId::MenuSettings;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void OnlineTopPage::onConnectButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::DirectConnection;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void OnlineTopPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    f32 delay = button->getDelay();
    changeSection(SectionId::TitleFromMenu, Anim::Prev, delay);
}

} // namespace UI
