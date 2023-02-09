#include "ServicePackToolsPage.hh"

#include "game/ui/SectionManager.hh"
#include "game/system/RaceConfig.hh"

#include <sp/ThumbnailManager.hh>

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

    initChildren(5);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_storageBenchmarkButton, 0);
    insertChild(2, &m_thumbnailsButton, 0);
    insertChild(3, &m_serverModeButton, 0);
    insertChild(4, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_storageBenchmarkButton.load("button", "ServicePackToolsButton", "StorageBenchmark", 0x1,
            false, false);
    m_thumbnailsButton.load("button", "ServicePackToolsButton", "Thumbnails", 0x1, false, false);
    m_serverModeButton.load("button", "ServicePackToolsButton", "ServerMode", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_storageBenchmarkButton.setFrontHandler(&m_onStorageBenchmarkButtonFront, false);
    m_thumbnailsButton.setFrontHandler(&m_onThumbnailsButtonFront, false);
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

void ServicePackToolsPage::onThumbnailsButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    if (SP::ThumbnailManager::Start()) {
        auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
        menuScenario.players[0].vehicleId = 1;
        menuScenario.players[0].characterId = 0;
        menuScenario.players[0].type = System::RaceConfig::Player::Type::Local;
        for (size_t i = 1; i < std::size(menuScenario.players); i++) {
            menuScenario.players[i].type = System::RaceConfig::Player::Type::None;
        }
        menuScenario.courseId = SP::ThumbnailManager::CourseId();
        menuScenario.engineClass = System::RaceConfig::EngineClass::CC150;
        menuScenario.gameMode = System::RaceConfig::GameMode::TimeAttack;
        menuScenario.cameraMode = 0;
        menuScenario.mirror = false;
        menuScenario.teams = false;
        menuScenario.spMaxTeamSize = 1;
        menuScenario.mirrorRng = false;

        f32 delay = button->getDelay();
        changeSection(SectionId::Thumbnails, Anim::Next, delay);
    } else {
        Section *section = SectionManager::Instance()->currentSection();
        auto *messagePage = section->page<PageId::MenuMessage>();
        messagePage->reset();
        messagePage->setTitleMessage(10313);
        messagePage->setWindowMessage(10314);
        messagePage->m_handler = &m_onThumbnailsNoCoursePop;
        m_replacement = PageId::MenuMessage;
        f32 delay = button->getDelay();
        startReplace(Anim::Next, delay);
    }
}

void ServicePackToolsPage::onServerModeButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *messagePage = section->page<PageId::MenuMessage>();
    messagePage->reset();
    messagePage->setTitleMessage(20007);
    messagePage->setWindowMessage(20010);
    messagePage->m_handler = &m_onThumbnailsNoCoursePop;

    m_replacement = PageId::MenuMessage;
    f32 delay = button->getDelay();
    startReplace(Anim::None, delay);
}

void ServicePackToolsPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::ServicePackTop;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void ServicePackToolsPage::onThumbnailsNoCoursePop([[maybe_unused]] MessagePage *messagePage) {
    reinterpret_cast<MenuMessagePage *>(messagePage)->m_replacement = PageId::ServicePackTools;
}

} // namespace UI
