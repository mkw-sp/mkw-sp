#include "FriendRoomPage.hh"

#include "game/ui/Section.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/YesNoPage.hh"

namespace UI {

FriendRoomPage::FriendRoomPage() = default;

FriendRoomPage::~FriendRoomPage() = default;

// Base function: 0x805d820c
void FriendRoomPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);
    initChildren(5);

    insertChild(0, &m_commentButton, 0);
    insertChild(1, &m_closeButton, 0);
    insertChild(2, &m_registerButton, 0);
    insertChild(3, &m_backButton, 0);
    insertChild(4, &m_instructionText, 0);

    m_commentButton.load("button", "FriendRoom", "Comment", 1, false, false);
    m_closeButton.load("button", "FriendRoom", "Close", 1, false, false);
    m_registerButton.load("button", "FriendRoom", "Register", 1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 1, false, true);
    m_instructionText.load();

    m_commentButton.setFrontHandler(&m_onCommentButtonFront, false);
    m_commentButton.setSelectHandler(&m_onButtonSelect, false);
    m_closeButton.setFrontHandler(&m_onCloseButtonFront, false);
    m_closeButton.setSelectHandler(&m_onButtonSelect, false);
    m_registerButton.setFrontHandler(&m_onRegisterButtonFront, false);
    m_registerButton.setSelectHandler(&m_onButtonSelect, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.setSelectHandler(&m_onButtonSelect, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    m_commentButton.m_index = 0;
    m_closeButton.m_index = 1;
    m_registerButton.m_index = 2;
    m_backButton.m_index = 3;
    m_commentButton.selectDefault(0);
    m_instructionText.setMessage(4370, nullptr);
}

// Base function: 0x805d8444
void FriendRoomPage::onActivate() {
    m_commentButton.selectDefault(0);
    m_instructionText.setMessage(4370, nullptr);

    switch (m_roomRole) {
    case RoomRole::Host:
        m_closeButton.setPlayerFlags(1);
        m_closeButton.setVisible(true);
        break;
    case RoomRole::Player:
    // case RoomRole::Spectator:
        m_closeButton.setPlayerFlags(0);
        m_closeButton.setVisible(false);
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
void FriendRoomPage::onCommentButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    push(PageId::FriendRoomMessageSelectPage, Anim::Next);
    m_instructionText.setMessage(4371, 0);
}

// Base function: 0x805d930c
void FriendRoomPage::onCloseButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    push(PageId::FriendRoomMessageSelectPage, Anim::Next);
    m_instructionText.setMessage(4373, 0);
}

// Base function: 0x805d9154
void FriendRoomPage::onRegisterButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {}

// Base function: 0x805d9160
void FriendRoomPage::onBackButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    YesNoPagePopup *yesNoPage = SectionManager::Instance()->currentSection()->page<PageId::YesNoPopup>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 20019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, nullptr);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

// Base function: 0x805d92a0
void FriendRoomPage::onButtonSelect(PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    s32 messageIds[] = {4370, 4372, 4374, 0};
    m_instructionText.setMessage(messageIds[button->m_index], nullptr);
}

// Base function: 0x805d930c
void FriendRoomPage::onBack([[maybe_unused]] u32 localPlayerId) {
    YesNoPagePopup *yesNoPage = SectionManager::Instance()->currentSection()->page<PageId::YesNoPopup>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 20019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, nullptr);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

} // namespace UI
