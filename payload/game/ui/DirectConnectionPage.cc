#include "DirectConnectionPage.hh"

#include "game/ui/SectionManager.hh"
#include <sp/cs/RoomClient.hh>

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
    m_okButton.setFrontHandler(&m_onOkButtonFront, false);
    m_backspaceButton.setFrontHandler(&m_onBackspaceButtonFront, false);
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
    if (!m_editBox.isEmpty()) {
        m_editBox.remove();
        return;
    }

    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Next, 0.0f);
}

void DirectConnectionPage::onDigitButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_editBox.insert(button->m_index);
}

void DirectConnectionPage::onOkButtonFront(PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    u64 directCode = m_editBox.getNumber() - 0x3ed5142afa4755f;
    u32 ip = directCode & 0xFFFFFFFF;
    u16 port = directCode >> 32 & 0xFFFF;
    u16 passcode = directCode >> 48 & 0x7FF;

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    SP::RoomClient::CreateInstance(sectionId == SectionId::OnlineSingle ? 1 : 2, ip, port,
            passcode);

    m_replacement = PageId::FriendMatching;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

void DirectConnectionPage::onBackspaceButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_editBox.remove();
}

void DirectConnectionPage::onBackButtonFront(PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_replacement = PageId::OnlineTop;
    f32 delay = button->getDelay();
    startReplace(Anim::Next, delay);
}

} // namespace UI
