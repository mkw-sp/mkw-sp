#include "DirectConnectionPage.hh"

namespace UI {

DirectConnectionPage::DirectConnectionPage() = default;

DirectConnectionPage::~DirectConnectionPage() = default;

PageId DirectConnectionPage::getReplacement() {
    return m_replacement;
}

void DirectConnectionPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(5 + std::size(m_digitButtons));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_editBox, 0);
    insertChild(2, &m_backspaceButton, 0);
    insertChild(3, &m_okButton, 0);
    insertChild(4, &m_backButton, 0);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        insertChild(5 + i, &m_digitButtons[i], 0);
    }

    m_pageTitleText.load(false);
    m_editBox.load(18, "button", "RoomCodeEditBox", "EditBox", "RoomCodeEditBoxLetter", "Letter",
            0x1, false, false);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        char variant[0x20];
        snprintf(variant, std::size(variant), "Key%u", i);
        m_digitButtons[i].load("button", "RegisterFriendKeyboard", variant, 0x1, false, false);
    }
    m_backspaceButton.load("button", "RegisterFriendKeyboard", "KeyBackSpace", 0x1, false, false);
    m_okButton.load("button", "RegisterFriendKeyboard", "OK", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        m_digitButtons[i].setFrontHandler(&m_onDigitButtonFront, false);
    }
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        m_digitButtons[i].m_index = i;
    }

    m_pageTitleText.setMessage(20005);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        MessageInfo info{};
        info.intVals[0] = i;
        m_digitButtons[i].setMessageAll(5105, &info);
    }

    m_digitButtons[1].selectDefault(0);
}

void DirectConnectionPage::onActivate() {
    m_replacement = PageId::None;
}

void DirectConnectionPage::onBack([[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Next, 0.0f);
}

void DirectConnectionPage::onDigitButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_editBox.insert(button->m_index);
}

void DirectConnectionPage::onBackButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::OnlineTop;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

} // namespace UI
