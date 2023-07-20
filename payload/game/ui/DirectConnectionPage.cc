#include "DirectConnectionPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <sp/cs/RoomClient.hh>

extern "C" {
#include <sp/keyboard/Keyboard.h>
}

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

    initChildren(6 + std::size(m_digitButtons));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_editBox, 0);
    insertChild(2, &m_backspaceButton, 0);
    insertChild(3, &m_resetButton, 0);
    insertChild(4, &m_okButton, 0);
    insertChild(5, &m_backButton, 0);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        insertChild(6 + i, &m_digitButtons[i], 0);
    }

    m_pageTitleText.load(false);
    m_editBox.load(18, "button", "RoomCodeEditBox", "EditBox", "RoomCodeEditBoxLetter", "Letter",
            0x1, false, false);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        char variant[0x20];
        snprintf(variant, std::size(variant), "Key%u", i);
        m_digitButtons[i].load("button", "RoomCodeKeyboard", variant, 0x1, false, false);
    }
    m_backspaceButton.load("button", "RoomCodeKeyboard", "KeyBackSpace", 0x1, false, false);
    m_resetButton.load("button", "RoomCodeKeyboard", "KeyReset", 0x1, false, false);
    m_okButton.load("button", "RoomCodeKeyboard", "OK", 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < std::size(m_digitButtons); i++) {
        m_digitButtons[i].setFrontHandler(&m_onDigitButtonFront, false);
    }
    m_backspaceButton.setFrontHandler(&m_onBackspaceButtonFront, false);
    m_resetButton.setFrontHandler(&m_onResetButtonFront, false);
    m_okButton.setFrontHandler(&m_onOkButtonFront, false);
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

    m_okButton.setPlayerFlags(0);
    m_digitButtons[1].selectDefault(0);

    auto *saveManager = System::SaveManager::Instance();
    u32 directCodeHigh = saveManager->getSetting<SP::ClientSettings::Setting::RoomCodeHigh>();
    u32 directCodeLow = saveManager->getSetting<SP::ClientSettings::Setting::RoomCodeLow>();
    u64 directCode = static_cast<u64>(directCodeHigh) << 32 | directCodeLow;
    if (directCode != std::numeric_limits<u64>::max()) {
        m_editBox.setNumber(directCode);
        m_okButton.selectDefault(0);
    }
    // TODO: Some linked-list callback queue (?)
    if (SP_IsConsoleInputInit()) {
        SP_SetKeypressCallback(
                +[](char c, u8 mods, void *userdata) {
                    auto *page = reinterpret_cast<DirectConnectionPage *>(userdata);
                    const bool handled = page->onKeyCode(c, mods);
                    (void)handled; // Ignore for now, would be used in multi-layered input system.
                },
                this);
    }
}

void DirectConnectionPage::onDeinit() {
    if (SP_IsConsoleInputInit()) {
        SP_SetKeypressCallback(
                +[](char, u8, void *) {}, nullptr);
    }
    u64 directCode = m_editBox.isFull() ? m_editBox.getNumber() : std::numeric_limits<u64>::max();
    u32 directCodeHigh = directCode >> 32;
    u32 directCodeLow = directCode;
    auto *saveManager = System::SaveManager::Instance();
    saveManager->setSetting<SP::ClientSettings::Setting::RoomCodeHigh>(directCodeHigh);
    saveManager->setSetting<SP::ClientSettings::Setting::RoomCodeLow>(directCodeLow);
}

void DirectConnectionPage::onActivate() {
    m_replacement = PageId::None;
}

void DirectConnectionPage::onBack(u32 /* localPlayerId */) {
    if (!m_editBox.isEmpty()) {
        m_editBox.remove();
        m_okButton.setPlayerFlags(0);
        if (m_okButton.isSelected()) {
            m_digitButtons[1].select(0);
        }
        return;
    }

    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Prev, 0.0f);
}

void DirectConnectionPage::onDigitButtonFront(PushButton *button, u32 /* localPlayerId */) {
    m_editBox.insert(button->m_index);
    if (m_editBox.isFull()) {
        m_okButton.setPlayerFlags(1);
        m_okButton.select(0);
    }
}

void DirectConnectionPage::onBackspaceButtonFront(PushButton * /* button */,
        u32 /* localPlayerId */) {
    m_editBox.remove();
    m_okButton.setPlayerFlags(0);
}

void DirectConnectionPage::onResetButtonFront(PushButton * /* button */, u32 /* localPlayerId */) {
    m_editBox.reset();
    m_okButton.setPlayerFlags(0);
}

void DirectConnectionPage::onOkButtonFront(PushButton *button, u32 /* localPlayerId */) {
    u64 directCode = m_editBox.getNumber();
    if (directCode == 0) {
        m_replacement = PageId::RandomMatching;
        return startReplace(Anim::Next, 0);
    }

    if (directCode < 0x3ed5142afa4755f || directCode > 0xbed51428fa4755e) {
        auto *messagePage =
                SectionManager::Instance()->currentSection()->page<PageId::MenuMessage>();
        messagePage->reset();
        messagePage->setTitleMessage(20032);
        messagePage->setWindowMessage(20033);
        messagePage->m_handler = &m_onBadConnectCode;
        m_replacement = PageId::MenuMessage;
        f32 delay = button->getDelay();
        startReplace(Anim::Next, delay);
        return;
    }
    directCode -= 0x3ed5142afa4755f;
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

void DirectConnectionPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    m_replacement = PageId::OnlineTop;
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void DirectConnectionPage::onBadConnectCode(MessagePage *messagePage) {
    reinterpret_cast<MenuMessagePage *>(messagePage)->m_replacement = PageId::OnlineTop;
}

bool DirectConnectionPage::onKeyCode(char key, u8 mods) {
    if (key == kSimpleEvent_Backspace) {
        if (m_okButton.isSelected()) {
            m_digitButtons[1].select(0);
        }
        if (mods & kSimpleMods_CTRL) {
            // Note: This clears the entire line, including everything to the right of the cursor
            m_resetButton.onFront(0 /* ? */, 0 /* ? */);
        } else {
            m_backspaceButton.onFront(0 /* ? */, 0 /* ? */);
        }
        return true;
    }

    if (key == kSimpleEvent_Enter) {
        if (m_editBox.isFull()) {
            m_okButton.onFront(0 /* ? */, 0 /* ? */);
        }
        return true;
    }

    if (key == kSimpleEvent_ArrowL) {
        m_editBox.onLeft(0, 0 /* ? */);
        return true;
    }
    if (key == kSimpleEvent_ArrowR) {
        m_editBox.onRight(0, 0 /* ? */);
        return true;
    }

    if (key <= '9' && key >= '0') {
        const int digit = key - '0';
        assert(digit >= 0 && digit <= std::ssize(m_digitButtons));
        m_digitButtons[digit].select(0);
        m_digitButtons[digit].onFront(0 /* ? */, 0 /* ? */);
        return true;
    }

    // Did not consume this input
    return false;
}

} // namespace UI
