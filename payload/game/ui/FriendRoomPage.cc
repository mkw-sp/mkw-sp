#include "FriendRoomPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/YesNoPage.hh"

namespace UI {

FriendRoomPage::FriendRoomPage() = default;

FriendRoomPage::~FriendRoomPage() = default;

void FriendRoomPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);
    initChildren(5);

    insertChild(0, &m_messageButton, 0);
    m_messageButton.load("button", "FriendRoom", "Comment", 1, false, false);
    m_messageButton.m_index = 0;
    m_messageButton.setFrontHandler(&m_onMessageButtonFront, false);
    m_messageButton.setSelectHandler(&m_onButtonSelect, false);

    insertChild(1, &m_closeButton, 0);
    m_messageButton.load("button", "FriendRoom", "Close", 1, false, false);
    m_messageButton.m_index = 1;
    m_messageButton.setFrontHandler(&m_onCloseButtonFront, false);
    m_messageButton.setSelectHandler(&m_onButtonSelect, false);

    insertChild(2, &m_registerButton, 0);
    m_messageButton.load("button", "FriendRoom", "Register", 1, false, false);
    m_messageButton.m_index = 2;
    m_messageButton.setFrontHandler(&m_onRegisterButtonFront, false);
    m_messageButton.setSelectHandler(&m_onButtonSelect, false);

    insertChild(3, &m_backButton, 0);
    m_messageButton.load("button", "Back", "ButtonBack", 1, false, true);
    m_messageButton.m_index = 3;
    m_messageButton.setFrontHandler(&m_onBackButtonFront, false);
    m_messageButton.setSelectHandler(&m_onButtonSelect, false);

    insertChild(4, &m_instructionText, 0);
    m_instructionText.load();

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_messageButton.selectDefault(0);
    m_instructionText.setMessage(4370, nullptr);
    // TODO: 80622cb8 is not in symbols.txt and I'm too lazy to fix it
    // I think this function's name is createGlobalContext, so I'll use that
    // SectionManager::s_instance->m_currentSection->createGlobalContext();
}

// Base function: 0x805d8444
void FriendRoomPage::onActivate() {
    m_maybePageState = 0;
    m_somethingRegister = false;
    m_messageButton.selectDefault(0);
    m_instructionText.setMessage(4370, nullptr);

    switch (m_roomRole) {
    case RoomRole::Host:
        m_closeButton.setPlayerFlags(1);
        m_closeButton.setVisible(false);
        break;
    case RoomRole::Player:
    // case RoomRole::Spectator:
        m_closeButton.setPlayerFlags(0);
        m_closeButton.setVisible(true);
        break;
    default:
        break;
    }

    m_registerButton.setPlayerFlags(0);
    m_registerButton.setVisible(false);
}

// Base function: 0x805d84fc
void FriendRoomPage::onDeactivate() {
    m_roomRole = RoomRole::None;
}

// Base function: 0x805d8508
void FriendRoomPage::afterCalc() {
    // TODO: this is where all of the online requests happen
}

// Base function: 0x805d8c98
void FriendRoomPage::onRefocus() {
    // TODO: this function
}

// Base function: 0x805d8f84
void FriendRoomPage::onMessageButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    // TODO: FriendRoomMessageSelectPage class doesn't exist yet!
    Page *messageSelectPage = SectionManager::Instance()->currentSection()->page(PageId::FriendRoomMessageSelectPage)->downcast<Page>();
    push(PageId::FriendRoomMessageSelectPage, Anim::Next);
    m_instructionText.setMessage(4371, 0);
}

// Base function: 0x805d930c
void FriendRoomPage::onCloseButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    // TODO: FriendRoomMessageSelectPage class doesn't exist yet!
    Page *messageSelectPage = SectionManager::Instance()->currentSection()->page(PageId::FriendRoomMessageSelectPage)->downcast<Page>();
    push(PageId::FriendRoomMessageSelectPage, Anim::Next);
    m_instructionText.setMessage(4373, 0);
}

// Base function: 0x805d9154
void FriendRoomPage::onRegisterButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    m_somethingRegister = true;
}

// Base function: 0x805d9160
void FriendRoomPage::onBackButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    YesNoPage *yesNoPage = SectionManager::Instance()->currentSection()->page(PageId::YesNoPopup)->downcast<YesNoPage>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 4019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, nullptr);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

// Base function: 0x805d92a0
void FriendRoomPage::onButtonSelect(PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    switch (button->m_index) {
    case 0:
        m_instructionText.setMessage(4370, nullptr);
        break;
    case 1:
        m_instructionText.setMessage(4372, nullptr);
        break;
    case 2:
        m_instructionText.setMessage(4374, nullptr);
        break;
    case 3:
        m_instructionText.setMessage(0, nullptr);
        break;
    default:
        break;
    }
}

// Base function: 0x805d930c
void FriendRoomPage::onBack([[maybe_unused]] u32 localPlayerId) {
    YesNoPage *yesNoPage = SectionManager::Instance()->currentSection()->page(PageId::YesNoPopup)->downcast<YesNoPage>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 4019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, nullptr);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

} // namespace UI
