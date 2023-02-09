#include "OnlineTeamSelectPage.hh"

#include "game/ui/SectionManager.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

OnlineTeamSelectPage::OnlineTeamSelectPage() = default;

OnlineTeamSelectPage::~OnlineTeamSelectPage() = default;

PageId OnlineTeamSelectPage::getReplacement() {
    return m_replacement;
}

void OnlineTeamSelectPage::onInit() {
    auto *context = SectionManager::Instance()->globalContext();
    m_localPlayerCount = context->m_localPlayerCount;

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
        m_buttons[i].load("button", "OnlineTeamSelect", variant, 0x0, false, false);
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);
    m_instructionText.load();

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        m_buttons[i].setFrontHandler(&m_onButtonFront, false);
    }
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (size_t i = 0; i < std::size(m_buttons); i++) {
        m_buttons[i].m_index = i;
    }

    m_pageTitleText.setMessage(3503);

    m_miiGroup.init(12, 1 << 2, nullptr);
    for (size_t i = 0; i < std::size(m_buttons); i++) {
        m_buttons[i].setMiiPicture("chara_shadow", &m_miiGroup, i, 2);
        m_buttons[i].setMiiPicture("chara", &m_miiGroup, i, 2);
        m_buttons[i].setMiiPicture("chara_light_01", &m_miiGroup, i, 2);
        m_buttons[i].setMiiPicture("chara_light_02", &m_miiGroup, i, 2);
    }
}

void OnlineTeamSelectPage::onActivate() {
    auto *roomManager = SP::RoomManager::Instance();
    u32 playerCount = roomManager->playerCount();
    for (u32 i = 0; i < playerCount; i++) {
        m_miiGroup.insertFromRaw(i, &roomManager->player(i).m_mii);
        MessageInfo info{};
        info.miis[0] = m_miiGroup.get(i);
        m_buttons[i].setMessageAll(9501, &info);
        m_buttons[i].refresh(0);
        m_buttons[i].setVisible(true);
        m_buttons[i].setPlayerFlags(0x0);
        for (u32 j = 0; j < m_localPlayerCount; j++) {
            if (roomManager->canSelectTeam(j, i)) {
                m_buttons[i].setPlayerFlags(1 << j);
                break;
            }
        }
    }
    for (u32 i = playerCount; i < 12; i++) {
        m_buttons[i].setVisible(false);
        m_buttons[i].setPlayerFlags(0x0);
    }

    for (u32 i = 0; i < m_localPlayerCount; i++) {
        for (u32 j = 0; j < playerCount; j++) {
            if (roomManager->canSelectTeam(i, j)) {
                m_buttons[j].selectDefault(i);
            }
        }
    }

    m_replacement = PageId::None;
}

void OnlineTeamSelectPage::onReceiveTeamSelect(u32 playerId, u32 teamId) {
    if (!SP::RoomManager::Instance()->canSelectTeam(playerId)) {
        m_buttons[playerId].refresh(teamId);
    }
}

void OnlineTeamSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {}

void OnlineTeamSelectPage::onButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    u32 playerId = button->m_index;
    auto *roomClient = SP::RoomClient::Instance();
    roomClient->sendTeamSelect(playerId);
    m_buttons[playerId].refresh(roomClient->player(playerId).m_teamId);
}

void OnlineTeamSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {}

} // namespace UI
