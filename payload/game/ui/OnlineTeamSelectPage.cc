#include "OnlineTeamSelectPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

OnlineTeamSelectPage::OnlineTeamSelectPage() = default;

OnlineTeamSelectPage::~OnlineTeamSelectPage() = default;

PageId OnlineTeamSelectPage::getReplacement() {
    return m_replacement;
}

void OnlineTeamSelectPage::onInit() {
    auto *context = SectionManager::Instance()->globalContext();
    m_localPlayerCount = context->m_localPlayerCount;
    //if (SectionManager::Instance()->currentSection()->id() == SectionId::OnlineServer) {
        m_localPlayerCount = 0;
    //}

    m_inputManager.init((1 << m_localPlayerCount) - 1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(2 + std::size(m_teamBoxControls));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_backButton, 0);
    for (size_t i = 0; i < std::size(m_teamBoxControls); i++) {
        insertChild(2 + i, &m_teamBoxControls[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < std::size(m_teamBoxControls); i++) {
        m_teamBoxControls[i].load();
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3503);
}

void OnlineTeamSelectPage::onActivate() {
    for (size_t i = 0; i < std::size(m_teamBoxControls); i++) {
        m_teamBoxControls[i].refresh(i, 18 + i);
    }

    /*for (size_t i = 0; i < m_localPlayerCount; i++) {
        m_controls[i].selectDefault(i);
    }*/

    m_replacement = PageId::None;
}

void OnlineTeamSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {}

void OnlineTeamSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {}

} // namespace UI
