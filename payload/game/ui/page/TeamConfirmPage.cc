#include "TeamConfirmPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MenuPage.hh"
#include "game/ui/page/SettingsPage.hh"

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

namespace UI {

TeamConfirmPage::TeamConfirmPage() = default;

TeamConfirmPage::~TeamConfirmPage() = default;

PageId TeamConfirmPage::getReplacement() {
    return m_replacement;
}

void TeamConfirmPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(4 + std::size(m_controls));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_okButton, 0);
    insertChild(3, &m_backButton, 0);
    for (size_t i = 0; i < std::size(m_controls); i++) {
        insertChild(4 + i, &m_controls[i], 0);
    }

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "RuleSetting", "ButtonRuleSetting", 0x1, false, false);
    for (size_t i = 0; i < std::size(m_controls); i++) {
        m_controls[i].load();
    }
    m_okButton.load("button", "TeamConfirmOK", "ButtonOK", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_okButton.setFrontHandler(&m_onOkButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3503);
    m_settingsButton.setMessageAll(10076);
}

void TeamConfirmPage::onActivate() {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    u32 maxTeamSize = menuScenario.spMaxTeamSize;
    u32 teamSizes[6]{};
    for (u32 playerId = 0; playerId < 12; playerId++) {
        u32 characterId = menuScenario.players[playerId].characterId;
        u32 teamId = menuScenario.players[playerId].spTeam;
        u32 positionId = (maxTeamSize == 6 ? 0 : 5 - maxTeamSize) * 12;
        positionId += teamId * maxTeamSize + teamSizes[teamId]++;
        m_controls[playerId].refresh(playerId, characterId, teamId, positionId);
    }

    menuScenario.teams = 0;
    m_okButton.selectDefault(0);
    m_replacement = PageId::None;
}

void TeamConfirmPage::prepareOfflineSingle() {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();

    u32 maxTeamSize = menuScenario.spMaxTeamSize;
    u32 playerCount = 12;
    u32 teamCount = (playerCount + maxTeamSize - 1) / maxTeamSize;
    maxTeamSize = (playerCount + teamCount - 1) / teamCount;
    assert(teamCount <= 6);
    u32 teamSizes[6]{};
    for (u32 i = 0; i < playerCount; i++) {
        do {
            menuScenario.players[i].spTeam = hydro_random_uniform(teamCount);
        } while (teamSizes[menuScenario.players[i].spTeam] >= maxTeamSize);
        teamSizes[menuScenario.players[i].spTeam]++;
    }
}

TeamConfirmPage *TeamConfirmPage::Get(PageId id) {
    if (id != PageId::TeamConfirm) {
        return nullptr;
    }

    auto *section = SectionManager::Instance()->currentSection();
    return section->page<PageId::TeamConfirm>();
}

void TeamConfirmPage::onBack(u32 /* localPlayerId */) {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::Multi) {
        m_replacement = PageId::MultiTeamSelect;
    } else {
        m_replacement = PageId::CharacterSelect;
    }
    startReplace(Anim::Prev, 0.0f);
}

void TeamConfirmPage::onSettingsButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, PageId::TeamConfirm);
    m_replacement = PageId::MenuSettings;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void TeamConfirmPage::onOkButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineVS) {
        System::RaceConfig::Instance()->applyEngineClass();
    }

    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    auto pageId =
            sectionId == SectionId::Multi ? PageId::MultiVehicleSelect : PageId::VehicleSelect;
    auto *characterSelectPage = section->page(pageId)->downcast<MenuPage>();
    characterSelectPage->m_prevId = PageId::TeamConfirm;
    m_replacement = pageId;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void TeamConfirmPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::Multi) {
        m_replacement = PageId::MultiTeamSelect;
    } else {
        m_replacement = PageId::CharacterSelect;
    }
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
