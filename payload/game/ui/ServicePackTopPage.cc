#include "ServicePackTopPage.hh"

#include "game/ui/SectionManager.hh"

#include <new>

namespace UI {

ServicePackTopPage::ServicePackTopPage() = default;

ServicePackTopPage::~ServicePackTopPage() = default;

PageId ServicePackTopPage::getReplacement() {
    return m_replacement;
}

void ServicePackTopPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(5);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_updateButton, 0);
    insertChild(2, &m_channelButton, 0);
    insertChild(3, &m_aboutButton, 0);
    insertChild(4, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_updateButton.load("button", "ServicePackTopButton", "Update", 0x1, false, false);
    m_channelButton.load("button", "ServicePackTopButton", "Channel", 0x1, false, false);
    m_aboutButton.load("button", "ServicePackTopButton", "About", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_updateButton.setFrontHandler(&m_onUpdateButtonFront, false);
    m_channelButton.setFrontHandler(&m_onChannelButtonFront, false);
    m_aboutButton.setFrontHandler(&m_onAboutButtonFront, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(10083);

    m_updateButton.selectDefault(0);
}

void ServicePackTopPage::onActivate() {
    m_replacement = PageId::None;
}

void ServicePackTopPage::onBack([[maybe_unused]] u32 localPlayerId) {
    changeSection(SectionId::TitleFromOptions, Anim::Prev, 0.0f);
}

void ServicePackTopPage::onUpdateButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::Update;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void ServicePackTopPage::onChannelButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::Channel;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void ServicePackTopPage::onAboutButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *confirmPage = section->page<PageId::Confirm>();
    confirmPage->reset();
    confirmPage->setTitleMessage(10089);
    wchar_t version[0x20];
    swprintf(version, std::size(version), L"MKW-SP v%s", versionInfo.name);
    MessageInfo info{};
    info.strings[0] = version;
    confirmPage->setWindowMessage(6602, &info);
    confirmPage->m_confirmHandler = &m_onAboutPop;
    confirmPage->m_cancelHandler = &m_onAboutPop;
    m_replacement = PageId::Confirm;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void ServicePackTopPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    f32 delay = button->getDelay();
    changeSection(SectionId::TitleFromOptions, Anim::Prev, delay);
}

void ServicePackTopPage::onAboutPop([[maybe_unused]] ConfirmPage *confirmPage,
        [[maybe_unused]] f32 delay) {
    confirmPage->m_replacement = PageId::ServicePackTop;
}

} // namespace UI
