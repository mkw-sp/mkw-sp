#include "CourseSelectPage.hh"

namespace UI {

CourseSelectPage::CourseSelectPage() = default;

CourseSelectPage::~CourseSelectPage() = default;

void CourseSelectPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(2);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_backButton, 0);

    m_pageTitleText.load(false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3405);
}

void CourseSelectPage::onActivate() {
    m_replacement = PageId::None;
}

void CourseSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::DriftSelect;
    startReplace(Anim::Prev, 0.0f);
}

void CourseSelectPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::DriftSelect;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
