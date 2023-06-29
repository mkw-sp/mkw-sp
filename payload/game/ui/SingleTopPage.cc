#include "SingleTopPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/CourseSelectPage.hh"
#include "game/ui/ModelPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/page/BattleModeSelectPage.hh"
#include "game/ui/page/MenuPage.hh"

#include <sp/settings/ClientSettings.hh>
#include <sp/trackPacks/TrackPackManager.hh>
extern "C" {
#include <vendor/libhydrogen/hydrogen.h>
}

using namespace magic_enum::bitwise_operators;

namespace UI {

SingleTopPage::SingleTopPage() = default;

SingleTopPage::~SingleTopPage() = default;

PageId SingleTopPage::getReplacement() {
    return m_replacement;
}

void SingleTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

#if ENABLE_MISSION_MODE
    initChildren(8);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_taButton, 0);
    insertChild(3, &m_vsButton, 0);
    insertChild(4, &m_btButton, 0);
    insertChild(5, &m_mrButton, 0);
    insertChild(6, &m_instructionText, 0);
    insertChild(7, &m_backButton, 0);
#else
    initChildren(7);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_settingsButton, 0);
    insertChild(2, &m_taButton, 0);
    insertChild(3, &m_vsButton, 0);
    insertChild(4, &m_btButton, 0);
    insertChild(5, &m_instructionText, 0);
    insertChild(6, &m_backButton, 0);
#endif

    m_pageTitleText.load(false);
    m_settingsButton.load("button", "SettingsButton", "Option", 0x1, false, false);
#if ENABLE_MISSION_MODE
    m_taButton.load("button", "SingleTop", "ButtonTA", 0x1, false, false);
    m_vsButton.load("button", "SingleTop", "ButtonVS", 0x1, false, false);
    m_btButton.load("button", "SingleTop", "ButtonBT", 0x1, false, false);
    m_mrButton.load("button", "SingleTop", "ButtonMR", 0x1, false, false);
#else
    m_taButton.load("button", "SingleTopNoMR", "ButtonTA", 0x1, false, false);
    m_vsButton.load("button", "SingleTopNoMR", "ButtonVS", 0x1, false, false);
    m_btButton.load("button", "SingleTopNoMR", "ButtonBT", 0x1, false, false);
#endif
    m_instructionText.load();
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_settingsButton.setSelectHandler(&m_onSettingsButtonSelect, false);
    m_taButton.setFrontHandler(&m_onTAButtonFront, false);
    m_taButton.setSelectHandler(&m_onTAButtonSelect, false);
    m_vsButton.setFrontHandler(&m_onVSButtonFront, false);
    m_vsButton.setSelectHandler(&m_onVSButtonSelect, false);
    m_btButton.setFrontHandler(&m_onBTButtonFront, false);
    m_btButton.setSelectHandler(&m_onBTButtonSelect, false);
#if ENABLE_MISSION_MODE
    m_mrButton.setFrontHandler(&m_onMRButtonFront, false);
    m_mrButton.setSelectHandler(&m_onMRButtonSelect, false);
#endif
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.setSelectHandler(&m_onBackButtonSelect, false);

    m_pageTitleText.setMessage(2011);

    m_taButton.selectDefault(0);
    m_instructionText.setMessage(3051);

    SectionManager::Instance()->globalContext()->clearCourses();
}

void SingleTopPage::onActivate() {
    m_replacement = PageId::None;

    // Reset button to TT when going from PackSelect, to fix possible
    // mismatches between the ModelPage and/or future concerns.
    if (anim() == Anim::Next) {
        m_taButton.selectDefault(0);
        onTAButtonSelect(&m_taButton, 0);
    }

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.itemMode = 0;
    menuScenario.teams = false;
    menuScenario.mirror = false;
    menuScenario.spMaxTeamSize = 1;
    for (u32 i = 0; i < 12; i++) {
        menuScenario.players[i].team = 2;
    }

    auto &trackPack = SP::TrackPackManager::Instance().getSelectedPack();
    auto packModes = trackPack.getSupportedModes();

    auto raceEnabled = (packModes & SP::Track::Mode::Race) == SP::Track::Mode::Race;
    auto coinEnabled = (packModes & SP::Track::Mode::Coin) == SP::Track::Mode::Coin;
    auto balloonEnabled = (packModes & SP::Track::Mode::Balloon) == SP::Track::Mode::Balloon;

    m_taButton.setVisible(raceEnabled);
    m_taButton.setPlayerFlags(raceEnabled);

    m_vsButton.setVisible(raceEnabled);
    m_vsButton.setPlayerFlags(raceEnabled);

    m_btButton.setVisible(coinEnabled || balloonEnabled);
    m_btButton.setPlayerFlags(coinEnabled || balloonEnabled);

#if ENABLE_MISSION_MODE
    auto isVanilla = SectionManager::Instance()->globalContext()->isVanillaTracks();

    // Hide the mission mode button for Track Packs.
    m_mrButton.setVisible(isVanilla);
    m_mrButton.setPlayerFlags(isVanilla);
#endif

    if (raceEnabled) {
        m_taButton.selectDefault(0);
        onTAButtonSelect(&m_taButton, 0);
    } else if (coinEnabled || balloonEnabled) {
        m_btButton.selectDefault(0);
        onBTButtonSelect(&m_btButton, 0);
    }
}

void SingleTopPage::onBack(u32 /* localPlayerId */) {
    onBackCommon(0);
}

void SingleTopPage::onSettingsButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *section = SectionManager::Instance()->currentSection();
    auto *settingsPage = section->page<PageId::MenuSettings>();
    settingsPage->configure(nullptr, PageId::SingleTop);

    m_replacement = PageId::MenuSettings;
    startReplace(Anim::Next, button->getDelay());
}

void SingleTopPage::onSettingsButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(10077);
}

void SingleTopPage::onTAButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *raceConfig = System::RaceConfig::Instance();
    auto &menuScenario = raceConfig->menuScenario();

    menuScenario.gameMode = System::RaceConfig::GameMode::TimeAttack;
    menuScenario.cameraMode = 0;

    raceConfig->applyEngineClass();
    raceConfig->applyPlayers();

    Section *section = SectionManager::Instance()->currentSection();
    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter();

    auto *characterSelectPage = section->page(PageId::CharacterSelect)->downcast<MenuPage>();
    assert(characterSelectPage);
    characterSelectPage->m_prevId = PageId::SingleTop;

    m_replacement = PageId::CharacterSelect;
    startReplace(Anim::Next, button->getDelay());
}

void SingleTopPage::onTAButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(3051);
    ModelPage::SetModel(0);
}

void SingleTopPage::onVSButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *raceConfig = System::RaceConfig::Instance();
    auto *saveManager = System::SaveManager::Instance();
    auto *context = SectionManager::Instance()->globalContext();
    context->m_matchCount = saveManager->getSetting<SP::ClientSettings::Setting::VSRaceCount>();

    auto maxTeamSizeSetting = saveManager->getSetting<SP::ClientSettings::Setting::VSTeamSize>();

    u32 maxTeamSize = SP::ClientSettings::GenerateMaxTeamSize(maxTeamSizeSetting);

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
    characterSelectPage->m_prevId = PageId::SingleTop;

    m_replacement = PageId::CharacterSelect;
    startReplace(Anim::Next, button->getDelay());
}

void SingleTopPage::onVSButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(3052);
    ModelPage::SetModel(2);
}

void SingleTopPage::onBTButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *saveManager = System::SaveManager::Instance();
    auto *context = SectionManager::Instance()->globalContext();
    context->m_matchCount = saveManager->getSetting<SP::ClientSettings::Setting::BTRaceCount>();

    auto maxTeamSizeSetting = saveManager->getSetting<SP::ClientSettings::Setting::BTTeamSize>();

    u32 maxTeamSize = SP::ClientSettings::GenerateMaxTeamSize(maxTeamSizeSetting);

    auto *raceConfig = System::RaceConfig::Instance();
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
    battleModeSelectPage->m_prevId = PageId::SingleTop;

    m_replacement = PageId::BattleModeSelect;
    startReplace(Anim::Next, button->getDelay());
}

void SingleTopPage::onBTButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(3054);
    ModelPage::SetModel(3);
}

#if ENABLE_MISSION_MODE
void SingleTopPage::onMRButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *raceConfig = System::RaceConfig::Instance();
    auto &menuScenario = raceConfig->menuScenario();

    menuScenario.gameMode = System::RaceConfig::GameMode::Mission;
    menuScenario.cameraMode = 0;

    raceConfig->applyPlayers();

    Section *section = SectionManager::Instance()->currentSection();
    auto *courseSelectPage = section->page<PageId::CourseSelect>();
    courseSelectPage->filter();

    auto *missionLevelSelectPage = section->page(PageId::MissionLevelSelect)->downcast<MenuPage>();
    assert(missionLevelSelectPage);
    missionLevelSelectPage->m_prevId = PageId::SingleTop;

    m_replacement = PageId::MissionLevelSelect;
    startReplace(Anim::Next, button->getDelay());
}

void SingleTopPage::onMRButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(10161);
    ModelPage::SetModel(2 /* for now */);
}
#endif

void SingleTopPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    onBackCommon(button->getDelay());
}

void SingleTopPage::onBackCommon(f32 delay) {
    m_replacement = PageId::PackSelect;
    startReplace(Anim::Prev, delay);
}

void SingleTopPage::onBackButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
    m_instructionText.setMessage(0);
}

} // namespace UI
