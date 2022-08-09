#include "ServicePackToolsPage.hh"

namespace UI {

ServicePackToolsPage::ServicePackToolsPage() = default;

ServicePackToolsPage::~ServicePackToolsPage() = default;

PageId ServicePackToolsPage::getReplacement() {
    return m_replacement;
}

void ServicePackToolsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(4);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_storageBenchmarkButton, 0);
    insertChild(2, &m_serverModeButton, 0);
    insertChild(3, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_storageBenchmarkButton.load("button", "ServicePackToolsButton", "StorageBenchmark", 0x1,
            false, false);
    m_serverModeButton.load("button", "ServicePackToolsButton", "ServerMode", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_storageBenchmarkButton.setFrontHandler(&m_onStorageBenchmarkButtonFront, false);
    m_serverModeButton.setFrontHandler(&m_onServerModeButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(20006);

    m_storageBenchmarkButton.selectDefault(0);
}

void ServicePackToolsPage::onActivate() {
    m_replacement = PageId::None;
}

void ServicePackToolsPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::ServicePackTop;
    startReplace(Anim::Prev, 0.0f);
}

void ServicePackToolsPage::onStorageBenchmarkButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::StorageBenchmark;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void ServicePackToolsPage::onServerModeButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    f32 delay = button->getDelay();
    changeSection(SectionId::OnlineServer, Anim::Next, delay);
}

void ServicePackToolsPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::ServicePackTop;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
