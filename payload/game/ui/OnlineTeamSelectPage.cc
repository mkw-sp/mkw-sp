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
    if (SectionManager::Instance()->currentSection()->id() == SectionId::OnlineServer) {
        m_localPlayerCount = 0;
    }

    m_inputManager.init((1 << m_localPlayerCount) - 1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(2 + std::size(m_controls));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_backButton, 0);
    for (size_t i = 0; i < std::size(m_controls); i++) {
        insertChild(2 + i, &m_controls[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < std::size(m_controls); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "UpDown%u", i);
        m_controls[i].load(6, 0, "control", "OnlineTeamUpDownBase", variant,
                "OnlineTeamUpDownButtonR", "RightButton", "OnlineTeamUpDownButtonL", "LeftButton",
                m_values[i].animator(), 0x1, false, false, true, true);
        m_values[i].load("ranking", "OnlineTeamUpDownValue", "Value", "OnlineTeamUpDownText",
                "Text");
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < std::size(m_values); i++) {
        m_values[i].setChangeHandler(&m_onValueChange);
    }
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3503);
    for (size_t i = 0; i < std::size(m_controls); i++) {
        m_controls[i].setMessageAll(10301);
    }
}

void OnlineTeamSelectPage::onActivate() {
    for (size_t i = 0; i < m_localPlayerCount; i++) {
        m_controls[i].selectDefault(i);
    }

    m_replacement = PageId::None;
}

void OnlineTeamSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {}

void OnlineTeamSelectPage::onValueChange([[maybe_unused]] TextUpDownValueControl::TextControl *text,
        [[maybe_unused]] u32 index) {
    text->setMessageAll(10268 + index);
}

void OnlineTeamSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {}

} // namespace UI
