#include "FriendRoomPage.hh"

#include "game/ui/FriendRoomMessageSelectPage.hh"
#include "game/ui/Section.hh"
#include "game/ui/SectionManager.hh"

#include <sp/cs/RoomClient.hh>
#include <sp/cs/RoomServer.hh>

namespace UI {

FriendRoomPage::FriendRoomPage() = default;

FriendRoomPage::~FriendRoomPage() = default;

void FriendRoomPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(7);
    insertChild(0, &m_settingsButton, 0);
    insertChild(1, &m_commentButton, 0);
    insertChild(2, &m_rulesButton, 0);
    insertChild(3, &m_closeButton, 0);
    insertChild(4, &m_registerButton, 0);
    insertChild(5, &m_backButton, 0);
    insertChild(6, &m_instructionText, 0);

    m_settingsButton.load("button", "SettingsButton", "Option", 0x1, false, false);
    m_commentButton.load("button", "FriendRoom", "Comment", 1, false, false);
    m_rulesButton.load("button", "FriendRoom", "Rules", 1, false, false);
    m_closeButton.load("button", "FriendRoom", "Close", 1, false, false);
    m_registerButton.load("button", "FriendRoom", "Register", 1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 1, false, true);
    m_instructionText.load();

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_settingsButton.setFrontHandler(&m_onSettingsButtonFront, false);
    m_settingsButton.setSelectHandler(&m_onButtonSelect, false);
    m_commentButton.setFrontHandler(&m_onCommentButtonFront, false);
    m_commentButton.setSelectHandler(&m_onButtonSelect, false);
    m_rulesButton.setFrontHandler(&m_onRulesButtonFront, false);
    m_rulesButton.setSelectHandler(&m_onButtonSelect, false);
    m_closeButton.setFrontHandler(&m_onCloseButtonFront, false);
    m_closeButton.setSelectHandler(&m_onButtonSelect, false);
    m_registerButton.setFrontHandler(&m_onRegisterButtonFront, false);
    m_registerButton.setSelectHandler(&m_onButtonSelect, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.setSelectHandler(&m_onButtonSelect, false);

    m_settingsButton.m_index = 20022;
    m_commentButton.m_index = 4370;
    m_rulesButton.m_index = 20024;
    m_closeButton.m_index = 4372;
    m_registerButton.m_index = 4374;
    m_backButton.m_index = 0;
}

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

void FriendRoomPage::onDeactivate() {
    m_roomRole = RoomRole::None;
}

void FriendRoomPage::pop() {
    startReplace(Anim::Prev, 0.0f);
}

void FriendRoomPage::onBack([[maybe_unused]] u32 localPlayerId) {
    YesNoPagePopup *yesNoPage = SectionManager::Instance()->currentSection()->page<PageId::YesNoPopup>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 20019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, &m_onBackConfirm);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

void FriendRoomPage::onSettingsButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(0);

    auto *section = SectionManager::Instance()->currentSection();
    auto *settingsPagePopup = section->page<PageId::SettingsPopup>();
    settingsPagePopup->configure(&m_onSettingsBack);
    push(PageId::SettingsPopup, Anim::Next);
}

void FriendRoomPage::onCommentButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    FriendRoomMessageSelectPage *messageSelectPage = SectionManager::Instance()->currentSection()->page<PageId::FriendRoomMessageSelect>();
    messageSelectPage->setMenuType(FriendRoomMessageSelectPage::MenuType::Comment);

    push(PageId::FriendRoomMessageSelect, Anim::Next);
    m_instructionText.setMessage(4371, 0);
}

void FriendRoomPage::onRulesButtonFront([[maybe_unused]] PushButton *button,
        [[maybe_unused]] u32 localPlayerId) {
    push(PageId::FriendRoomRules, Anim::Next);
}

void FriendRoomPage::onCloseButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    FriendRoomMessageSelectPage *messageSelectPage = SectionManager::Instance()->currentSection()->page<PageId::FriendRoomMessageSelect>();
    messageSelectPage->setMenuType(FriendRoomMessageSelectPage::MenuType::Close);

    push(PageId::FriendRoomMessageSelect, Anim::Next);
    m_instructionText.setMessage(4373, 0);
}

void FriendRoomPage::onRegisterButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {}

void FriendRoomPage::onBackButtonFront([[maybe_unused]] PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    YesNoPagePopup *yesNoPage = SectionManager::Instance()->currentSection()->page<PageId::YesNoPopup>();

    yesNoPage->reset();
    yesNoPage->setWindowMessage((m_roomRole == RoomRole::Host) + 20019, nullptr);
    yesNoPage->configureButton(0, 4012, nullptr, Anim::None, &m_onBackConfirm);
    yesNoPage->configureButton(1, 4013, nullptr, Anim::None, nullptr);
    push(PageId::YesNoPopup, Anim::Next);
}

void FriendRoomPage::onButtonSelect(PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(button->m_index, nullptr);
}

void FriendRoomPage::onBackConfirm([[maybe_unused]] s32 choice, [[maybe_unused]] PushButton *button) {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::OnlineServer) {
        SP::RoomServer::DestroyInstance();
        changeSection(SectionId::ServicePack, Anim::Prev, 0.0f);
    } else {
        SP::RoomClient::DestroyInstance();
        startReplace(Anim::Prev, 0.0f);
    }
}

void FriendRoomPage::onSettingsBack([[maybe_unused]] SettingsPage *settingsPage,
        [[maybe_unused]] PushButton *button) {
    m_instructionText.setMessage(20022);

    if (auto *client = SP::RoomClient::Instance()) {
        client->changeLocalSettings();
    }
}

} // namespace UI
