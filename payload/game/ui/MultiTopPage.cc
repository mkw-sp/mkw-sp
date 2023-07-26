#include "MultiTopPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/CourseSelectPage.hh"
#include "game/ui/ModelPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/page/BattleModeSelectPage.hh"
#include "game/ui/page/MenuPage.hh"
#include <sp/settings/ClientSettings.hh>

extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

namespace UI {

MultiTopPage::MultiTopPage() = default;

MultiTopPage::~MultiTopPage() = default;

PageId MultiTopPage::getReplacement() {
    return m_replacement;
}

void MultiTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(6);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_vsButton, 0);
    insertChild(3, &m_btButton, 0);
    insertChild(4, &m_instructionText, 0);
    insertChild(5, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "SettingsButton", "Option", 0x1, false, false);
    m_vsButton.load("button", "MultiTop", "ButtonVS", 0x1, false, false);
    m_btButton.load("button", "MultiTop", "ButtonBT", 0x1, false, false);
    m_instructionText.load();
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_settingsButton.setSelectHandler(&m_onSettingsButtonSelect, false);
    m_vsButton.setFrontHandler(&m_onVSButtonFront, false);
    m_vsButton.setSelectHandler(&m_onVSButtonSelect, false);
    m_btButton.setFrontHandler(&m_onBTButtonFront, false);
    m_btButton.setSelectHandler(&m_onBTButtonSelect, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.setSelectHandler(&m_onBackButtonSelect, false);

    auto *context = SectionManager::Instance()->globalContext();
    m_pageTitleText.setMessage(2028 + context->m_localPlayerCount);

    m_reset = true;

    SectionManager::Instance()->globalContext()->clearCourses();
}

void MultiTopPage::onActivate() {
    m_replacement = PageId::None;

    if (m_reset) {
        m_vsButton.selectDefault(0);
        m_instructionText.setMessage(3052);

        ModelPage::SetModel(2);
        m_reset = false;
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.itemMode = 0;
    menuScenario.teams = false;
    menuScenario.mirror = false;
    menuScenario.mirrorRng = false;
    menuScenario.spMaxTeamSize = 1;
    for (u32 i = 0; i < 12; i++) {
        menuScenario.players[i].team = 2;
    }
}

void MultiTopPage::onBack(u32 /* localPlayerId */) {
    onBackCommon(0.0f);
}

void MultiTopPage::onSettingsButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *menuSettingsPage = section->page<PageId::MenuSettings>();
    menuSettingsPage->configure(nullptr, PageId::MultiTop);

    m_replacement = PageId::MenuSettings;
    startReplace(Anim::Next, button->getDelay());
}

void MultiTopPage::onSettingsButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(10077);
}

void MultiTopPage::onVSButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *raceConfig = System::RaceConfig::Instance();
    auto *saveManager = System::SaveManager::Instance();
    auto *context = SectionManager::Instance()->globalContext();
    context->m_matchCount = saveManager->getSetting<SP::ClientSettings::Setting::VSRaceCount>();

    auto teamsizeSetting = saveManager->getSetting<SP::ClientSettings::Setting::VSTeamSize>();
    u32 maxTeamSize = SP::ClientSettings::GenerateMaxTeamSize(teamsizeSetting);

    auto &menuScenario = raceConfig->menuScenario();
    menuScenario.gameMode = System::RaceConfig::GameMode::OfflineVS;
    menuScenario.spMaxTeamSize = maxTeamSize;
    menuScenario.cameraMode = 5;

    context->applyVehicleRestriction(false);
    raceConfig->applyPlayers();
    raceConfig->applyCPUMode();
    raceConfig->applyItemFreq();
    raceConfig->applyEngineClass();

    Section *section = SectionManager::Instance()->currentSection();
    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter();

    auto *characterSelectPage = section->page(PageId::CharacterSelect)->downcast<MenuPage>();
    assert(characterSelectPage);
    characterSelectPage->m_prevId = PageId::MultiTop;

    m_replacement = PageId::CharacterSelect;
    startReplace(Anim::Next, button->getDelay());
}

void MultiTopPage::onVSButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(3052);
    ModelPage::SetModel(2);
}

void MultiTopPage::onBTButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *raceConfig = System::RaceConfig::Instance();
    auto *saveManager = System::SaveManager::Instance();
    auto *context = SectionManager::Instance()->globalContext();
    context->m_matchCount = saveManager->getSetting<SP::ClientSettings::Setting::BTRaceCount>();

    auto teamsizeSetting = saveManager->getSetting<SP::ClientSettings::Setting::BTTeamSize>();
    u32 maxTeamSize = SP::ClientSettings::GenerateMaxTeamSize(teamsizeSetting);

    auto &menuScenario = raceConfig->menuScenario();
    menuScenario.gameMode = System::RaceConfig::GameMode::OfflineBT;
    menuScenario.spMaxTeamSize = maxTeamSize;
    menuScenario.cameraMode = 5;

    context->applyVehicleRestriction(true);
    raceConfig->applyPlayers();
    raceConfig->applyCPUMode();
    raceConfig->applyItemFreq();
    raceConfig->applyEngineClass();

    Section *section = SectionManager::Instance()->currentSection();
    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter();

    auto *battleModeSelectPage = section->page<PageId::BattleModeSelect>();
    assert(battleModeSelectPage);
    battleModeSelectPage->m_prevId = PageId::MultiTop;

    m_replacement = PageId::BattleModeSelect;
    startReplace(Anim::Next, button->getDelay());
}

void MultiTopPage::onBTButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(3054);
    ModelPage::SetModel(3);
}

void MultiTopPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    onBackCommon(button->getDelay());
}

void MultiTopPage::onBackCommon(f32 delay) {
    m_reset = true;

    m_replacement = PageId::PackSelect;
    startReplace(Anim::Prev, delay);
}

void MultiTopPage::onBackButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(0);
}

} // namespace UI
