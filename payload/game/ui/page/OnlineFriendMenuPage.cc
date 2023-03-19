#include "OnlineFriendMenuPage.hh"

#include "game/ui/GlobePage.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

enum class ButtonId {
    NewRoom,
    Roster,
    Register,
    Back,
};

static const char *animInfo[] = {"State", "Offline", "RandomMatching", "FriendParent", nullptr,
        "State2", "Offline2", "RandomMatching2", "FriendParent2", nullptr, nullptr};

PageId OnlineFriendMenuPage::getReplacement() {
    return m_replacement;
}

void OnlineFriendMenuPage::onInit() {
    m_inputManager.init(1, 0);
    setInputManager(&m_inputManager);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(6);
    insertChild(0, &m_titleText, 0);
    insertChild(1, &m_instructionText, 0);
    insertChild(2, &m_friendListButton, 0);
    insertChild(3, &m_registerButton, 0);
    insertChild(4, &m_startRoomButton, 0);
    insertChild(5, &m_backButton, 0);

    m_friendListButton.load(animInfo, "button", "WifiFriendMenuListButton", "ButtonList", 1, 0);
    m_friendListButton.setFrontHandler(&m_onFriendListFront, false);
    m_friendListButton.setSelectHandler(&m_onButtonSelect, false);
    m_friendListButton.m_animator.getGroup(4)->setAnimation(0, 0.0);
    m_friendListButton.m_animator.getGroup(5)->setAnimation(0, 0.0);
    m_friendListButton.m_index = static_cast<s32>(ButtonId::Roster);

    m_registerButton.load("button", "WifiFriendMenu", "ButtonRegister", 1, 0, false);
    m_registerButton.setFrontHandler(&m_onRegisterFront, false);
    m_registerButton.setSelectHandler(&m_onButtonSelect, false);
    m_registerButton.m_index = static_cast<s32>(ButtonId::Register);

    m_startRoomButton.load("button", "WifiFriendMenu", "ButtonNewRoom", 1, 0, false);
    m_startRoomButton.setFrontHandler(&m_onStartRoomFront, false);
    m_startRoomButton.setSelectHandler(&m_onButtonSelect, false);
    m_startRoomButton.m_index = static_cast<s32>(ButtonId::NewRoom);

    m_backButton.load("button", "Back", "ButtonBack", 1, 0, true);
    m_backButton.setFrontHandler(&m_onBackFront, false);
    m_backButton.setSelectHandler(&m_onButtonSelect, false);
    m_backButton.m_index = static_cast<s32>(ButtonId::Back);

    m_titleText.load(0);
    m_instructionText.load();
}

void OnlineFriendMenuPage::onActivate() {
    auto section = SectionManager::Instance()->currentSection();
    auto globePage = section->page<PageId::Globe>();

    globePage->requestSpinLocal(0);

    m_titleText.setMessage(4002);
    m_instructionText.setMessage(4317);
    m_friendListButton.selectDefault(0);
}

void OnlineFriendMenuPage::onDeactivate() {
    auto section = SectionManager::Instance()->currentSection();
    auto globePage = section->page<PageId::Globe>();

    globePage->requestSpinFar();
}

void OnlineFriendMenuPage::onFriendListFront(PushButton *button, u32 /* localPlayerId */) {
    m_replacement = PageId::OnlineFriendList;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineFriendMenuPage::onRegisterFront(PushButton *button, u32 /* localPlayerId */) {
    m_replacement = PageId::UnknownA5;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineFriendMenuPage::onStartRoomFront(PushButton * /* button */, u32 /* localPlayerId */) {}

void OnlineFriendMenuPage::onButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    m_instructionText.setMessage(4316 + button->m_index);
}

void OnlineFriendMenuPage::onBackFront(PushButton *button, u32 /* localPlayerId */) {
    m_replacement = PageId::OnlineTop;
    startReplace(Anim::Prev, button ? button->getDelay() : 0);
}

void OnlineFriendMenuPage::onBack(u32 localPlayerId) {
    onBackFront(nullptr, localPlayerId);
}

} // namespace UI
