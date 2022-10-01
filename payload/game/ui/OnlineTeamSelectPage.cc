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

    initChildren(3 + std::size(m_buttons));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_backButton, 0);
    insertChild(2, &m_instructionText, 0);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        insertChild(3 + i, &m_buttons[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        char variant[0x20];
        snprintf(variant, std::size(variant), "Member%02zu", i);
        m_buttons[i].load("button", "OnlineTeamSelect", variant,
                i < m_localPlayerCount ? 1 << i : 0, false, false);
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);
    m_instructionText.load();

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3503);

    m_miiGroup.init(12, 1 << 2, nullptr);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        m_buttons[i].setMiiPicture("chara_icon_sha", &m_miiGroup, i, 2);
        m_buttons[i].setMiiPicture("chara_icon", &m_miiGroup, i, 2);
    }
}

void OnlineTeamSelectPage::onActivate() {
    auto *context = SectionManager::Instance()->globalContext();
    for (size_t i = 0; i < 12; i++) {
        m_miiGroup.copy(&context->m_localPlayerMiis, 0, i);
    }

    for (size_t i = 0; i < std::size(m_buttons); i++) {
        MessageInfo info{};
        info.strings[0] = L"WWWWWWWWWW";
        m_buttons[i].setMessageAll(20026 + (i * 17) % 6, &info);
    }
    /*for (size_t i = 0; i < std::size(m_boxControls); i++) {
        m_boxControls[i].refresh(i, 18 + i);
    }
    for (size_t i = 0; i < std::size(m_miiIconControls); i++) {
        MessageInfo info{};
        info.strings[0] = L"WWWWWWWWWW";
        m_miiIconControls[i].setMessage("name", 6602, &info);
        //info.miis[0] = m_miiGroup.get(i);
        //m_miiIconControls[i].setMessage("name", 9501, &info);
        m_boxControls[i / 3].attach(&m_miiIconControls[i]);
    }*/

    for (size_t i = 0; i < m_localPlayerCount; i++) {
        m_buttons[i].selectDefault(i);
    }

    m_replacement = PageId::None;
}

void OnlineTeamSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {}

void OnlineTeamSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {}

} // namespace UI
