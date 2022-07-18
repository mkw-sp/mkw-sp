#include "SingleTopPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/MenuPage.hh"

namespace UI {

SingleTopPage::SingleTopPage() = default;

SingleTopPage::~SingleTopPage() = default;

PageId SingleTopPage::getReplacement() {
    return m_replacement;
}

void SingleTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Both);

#if ENABLE_MR
    initChildren(7);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_taButton, 0);
    insertChild(2, &m_vsButton, 0);
    insertChild(3, &m_btButton, 0);
    insertChild(4, &m_mrButton, 0);
    insertChild(5, &m_instructionText, 0);
    insertChild(6, &m_backButton, 0);
#else
    initChildren(6);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_taButton, 0);
    insertChild(2, &m_vsButton, 0);
    insertChild(3, &m_btButton, 0);
    insertChild(4, &m_instructionText, 0);
    insertChild(5, &m_backButton, 0);
#endif

    m_pageTitleText.load(false);
#if ENABLE_MR
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
    m_taButton.setFrontHandler(&m_onTAButtonFront, false);
    m_taButton.setSelectHandler(&m_onTAButtonSelect, false);
    m_vsButton.setFrontHandler(&m_onVSButtonFront, false);
    m_vsButton.setSelectHandler(&m_onVSButtonSelect, false);
    m_btButton.setFrontHandler(&m_onBTButtonFront, false);
    m_btButton.setSelectHandler(&m_onBTButtonSelect, false);
#if ENABLE_MR
    m_mrButton.setFrontHandler(&m_onMRButtonFront, false);
    m_mrButton.setSelectHandler(&m_onMRButtonSelect, false);
#endif
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.setSelectHandler(&m_onBackButtonSelect, false);

    m_pageTitleText.setMessage(2011);

    m_taButton.selectDefault(0);
    m_instructionText.setMessage(3051);

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(0);
}

void SingleTopPage::onActivate() {
    m_replacement = PageId::None;

    auto *context = SectionManager::Instance()->globalContext();
    context->_74 = 2;

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.itemMode = 0;
    menuScenario.teams = false;
    menuScenario.mirror = false;
    for (u32 i = 0; i < 12; i++) {
        menuScenario.players[i].team = 2;
    }
}

void SingleTopPage::onBack([[maybe_unused]] u32 localPlayerId) {
    changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
}

void SingleTopPage::onTAButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.engineClass = System::RaceConfig::EngineClass::CC150;
    menuScenario.gameMode = System::RaceConfig::GameMode::TimeAttack;
    menuScenario.cameraMode = 0;
    menuScenario.players[0].type = System::RaceConfig::Player::Type::Local;
    for (u32 i = 1; i < 12; i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }

    Section *section = SectionManager::Instance()->currentSection();
    auto *page = section->page(PageId::CharacterSelect)->downcast<MenuPage>();
    assert(page);
    page->m_prevId = PageId::SingleTop;

    m_replacement = PageId::CharacterSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void SingleTopPage::onTAButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(3051);

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(0);
}

void SingleTopPage::onVSButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.gameMode = System::RaceConfig::GameMode::OfflineVS;
    menuScenario.cameraMode = 5;
    menuScenario.players[0].type = System::RaceConfig::Player::Type::Local;
    for (u32 i = 1; i < 12; i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::CPU;
    }

    Section *section = SectionManager::Instance()->currentSection();
    auto *page = section->page(PageId::VsModeSelect)->downcast<MenuPage>();
    assert(page);
    page->m_prevId = PageId::SingleTop;

    m_replacement = PageId::VsModeSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void SingleTopPage::onVSButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(3052);

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(2);
}

void SingleTopPage::onBTButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.engineClass = System::RaceConfig::EngineClass::CC50;
    menuScenario.gameMode = System::RaceConfig::GameMode::OfflineBT;
    menuScenario.cameraMode = 5;
    menuScenario.players[0].type = System::RaceConfig::Player::Type::Local;
    for (u32 i = 1; i < 12; i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::CPU;
    }

    Section *section = SectionManager::Instance()->currentSection();
    auto *page = section->page(PageId::BattleModeSelect)->downcast<MenuPage>();
    assert(page);
    page->m_prevId = PageId::SingleTop;

    m_replacement = PageId::BattleModeSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void SingleTopPage::onBTButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(3054);

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(3);
}

#if ENABLE_MR
void SingleTopPage::onMRButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.gameMode = System::RaceConfig::GameMode::Mission;
    menuScenario.cameraMode = 0;
    menuScenario.players[0].type = System::RaceConfig::Player::Type::Local;
    for (u32 i = 1; i < 12; i++) {
        menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
    }

    Section *section = SectionManager::Instance()->currentSection();
    auto *page = section->page(PageId::MissionLevelSelect)->downcast<MenuPage>();
    assert(page);
    page->m_prevId = PageId::SingleTop;

    m_replacement = PageId::MissionLevelSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void SingleTopPage::onMRButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(10161);

    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->modelControl().setModel(2 /* for now */);
}
#endif

void SingleTopPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    f32 delay = button->getDelay();
    changeSection(SectionId::TitleFromMenu, Anim::Prev, delay);
}

void SingleTopPage::onBackButtonSelect([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(0);
}

} // namespace UI
