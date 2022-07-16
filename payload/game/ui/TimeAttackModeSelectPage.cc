#include "TimeAttackModeSelectPage.hh"

extern "C" {
#include "game/system/GhostFile.h"
}
#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

TimeAttackModeSelectPage::TimeAttackModeSelectPage() = default;

TimeAttackModeSelectPage::~TimeAttackModeSelectPage() = default;

PageId TimeAttackModeSelectPage::getReplacement() {
    return m_replacement;
}

void TimeAttackModeSelectPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(3 + std::size(m_buttons2) + std::size(m_buttons3));
    size_t index = 0;
    insertChild(index++, &m_pageTitleText, 0);
    insertChild(index++, &m_settingsButton, 0);
    for (size_t i = 0; i < std::size(m_buttons2); i++) {
        insertChild(index++, &m_buttons2[i], 0);
    }
    for (size_t i = 0; i < std::size(m_buttons3); i++) {
        insertChild(index++, &m_buttons3[i], 0);
    }
    insertChild(index++, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "RuleSetting", "ButtonRuleSetting", 0x1, false, false);
    for (size_t i = 0; i < std::size(m_buttons2); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Button2_%zu", i);
        m_buttons2[i].load("button", "TimeAttackModeSelect", variant, 0x1, false, false);
    }
    for (size_t i = 0; i < std::size(m_buttons3); i++) {
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Button3_%zu", i);
        m_buttons3[i].load("button", "TimeAttackModeSelect", variant, 0x1, false, false);
    }
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack);
    for (size_t i = 0; i < std::size(m_buttons2); i++) {
        m_buttons2[i].setFrontHandler(&m_onButtonFront, false);
    }
    for (size_t i = 0; i < std::size(m_buttons3); i++) {
        m_buttons3[i].setFrontHandler(&m_onButtonFront, false);
    }
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);

    m_pageTitleText.setMessage(10165);
    m_settingsButton.setMessageAll(10076);
}

void TimeAttackModeSelectPage::onActivate() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *ghostListPage = section->page<PageId::TimeAttackGhostList>();
    u32 chosenCount = ghostListPage->m_chosenCount;
    for (size_t i = 0; i < std::size(m_buttons2); i++) {
        m_buttons2[i].setVisible(chosenCount != 1);
        m_buttons2[i].setPlayerFlags(chosenCount != 1 ? 0x1 : 0x0);
    }
    for (size_t i = 0; i < std::size(m_buttons3); i++) {
        m_buttons3[i].setVisible(chosenCount == 1);
        m_buttons3[i].setPlayerFlags(chosenCount == 1 ? 0x1 : 0x0);
    }

    if (ghostListPage->m_chosenCount == 0) {
        m_buttons2[0].setMessageAll(10166);
        m_buttons2[1].setMessageAll(10167);
        m_buttons2[0].m_index = 0;
        m_buttons2[1].m_index = 1;
        m_buttons2[0].selectDefault(0);
    } else if (ghostListPage->m_chosenCount == 1) {
        m_buttons3[0].setMessageAll(10166);
        m_buttons3[1].setMessageAll(10167);
        m_buttons3[2].setMessageAll(10168);
        m_buttons3[0].m_index = 0;
        m_buttons3[1].m_index = 1;
        m_buttons3[2].m_index = 2;
        m_buttons3[0].selectDefault(0);
    } else {
        m_buttons2[0].setMessageAll(10166);
        m_buttons2[1].setMessageAll(10168);
        m_buttons2[0].m_index = 0;
        m_buttons2[1].m_index = 2;
        m_buttons2[0].selectDefault(0);
    }

    m_replacement = PageId::None;
}

void TimeAttackModeSelectPage::onRefocus() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *raceConfirmPage = section->page<PageId::RaceConfirm>();
    if (!raceConfirmPage->hasConfirmed()) {
        return;
    }

    auto *ghostListPage = section->page<PageId::TimeAttackGhostList>();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 playerCount = ghostListPage->m_chosenCount;
    if (playerCount == 0 || !m_isReplay) {
        playerCount++;
    }
    for (u32 i = playerCount; i < std::size(menuScenario.players); i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }

    GlobalContext *context = SectionManager::Instance()->globalContext();
    context->m_timeAttackGhostType = GHOST_TYPE_SAVED;
    context->m_timeAttackCourseId = menuScenario.courseId;
    context->m_timeAttackLicenseId = -1;
    context->m_timeAttackIsVanilla = m_isVanilla;

    context->m_timeAttackGhostCount = 0;
    for (u32 i = 0; i < ghostListPage->m_ghostIsChosen.size(); i++) {
        if (ghostListPage->m_ghostIsChosen[i]) {
            u32 ghostIndex = ghostListPage->m_ghostList->indices()[i];
            context->m_timeAttackGhostIndices[context->m_timeAttackGhostCount++] = ghostIndex;
        }
    }

    auto *ghostManagerPage = section->page<PageId::GhostManager>();
    SectionId sectionId;
    if (ghostListPage->m_chosenCount == 0) {
        sectionId = SectionId::TA;
    } else if (m_isReplay) {
        ghostManagerPage->requestGhostReplay();
        sectionId = SectionId::GhostReplay;
    } else {
        ghostManagerPage->requestGhostRace(false, false);
        sectionId = SectionId::TA;
    }
    changeSection(sectionId, Anim::Next, 0.0f);
}

void TimeAttackModeSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::TimeAttackGhostList;
    startReplace(Anim::Prev, 0.0f);
}

void TimeAttackModeSelectPage::onSettingsButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *settingsPage = section->page<PageId::Settings>();
    settingsPage->m_replacement = PageId::TimeAttackModeSelect;
    m_replacement = PageId::Settings;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void TimeAttackModeSelectPage::onButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    switch (button->m_index) {
    case 0:
        m_isVanilla = false,
        m_isReplay = false;
        break;
    case 1:
        m_isVanilla = true;
        m_isReplay = false;
        break;
    case 2:
        m_isVanilla = false;
        m_isReplay = true;
        break;
    default:
        return;
    }

    push(PageId::RaceConfirm, Anim::Next);
}

void TimeAttackModeSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::TimeAttackGhostList;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
