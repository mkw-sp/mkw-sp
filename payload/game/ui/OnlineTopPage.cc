#include "OnlineTopPage.hh"

#include "game/ui/MessagePage.hh"
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/YesNoPage.hh"

#include <sp/trackPacks/TrackPackManager.hh>

namespace UI {

enum ButtonId {
    Worldwide,
    Trackpack,
    Friend,
    Direct,
};

PageId OnlineTopPage::getReplacement() {
    return m_replacement;
}

// clang-format off
static const char *animInfo[] = {
    "State", "Offline", "RandomMatching", "FriendParent", nullptr,
    "State2", "Offline2", "RandomMatching2", "FriendParent2", nullptr,
    nullptr
};
// clang-format on

void OnlineTopPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);

    initChildren(7);
    insertChild(0, &m_worldwideButton, 0);
    insertChild(1, &m_trackpackButton, 0);
    insertChild(2, &m_friendButton, 0);
    insertChild(3, &m_directButton, 0);
    insertChild(4, &m_backButton, 0);
    insertChild(5, &m_pageTitleText, 0);
    insertChild(6, &m_instructionText, 0);

    m_worldwideButton.load("button", "WifiMenuSingleTop", "ButtonWorld", 1, 0, false);
    m_worldwideButton.setFrontHandler(&m_onWorldWideButtonFront, false);
    m_worldwideButton.setSelectHandler(&m_onButtonSelect, false);
    m_worldwideButton.m_index = ButtonId::Worldwide;

    m_trackpackButton.load("button", "WifiMenuSingleTop", "ButtonRegion", 1, 0, false);
    m_trackpackButton.setFrontHandler(&m_onTrackpackButtonFront, false);
    m_trackpackButton.setSelectHandler(&m_onButtonSelect, false);
    m_trackpackButton.m_index = ButtonId::Trackpack;

    m_friendButton.load(animInfo, "button", "WifiMenuSingleTopFriendButton", "ButtonFriend", 1, 0);
    m_friendButton.setFrontHandler(&m_onFriendButtonFront, false);
    m_friendButton.setSelectHandler(&m_onButtonSelect, false);
    m_friendButton.m_animator.getGroup(4)->setAnimation(0, 0.0);
    m_friendButton.m_animator.getGroup(5)->setAnimation(0, 0.0);
    m_friendButton.m_index = ButtonId::Friend;

    m_directButton.load("button", "WifiMenuSingleTop", "ButtonGhostBattle", 1, 0, false);
    m_directButton.setFrontHandler(&m_onDirectButtonFront, false);
    m_directButton.setSelectHandler(&m_onButtonSelect, false);
    m_directButton.m_index = ButtonId::Direct;

    m_backButton.load("button", "Back", "ButtonBack", 1, 0, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.load(false);
    m_instructionText.load();
}

void OnlineTopPage::onActivate() {
    m_replacement = PageId::None;
    m_worldwideButton.selectDefault(0);
    m_instructionText.setMessage(4310);

    if (SectionManager::Instance()->currentSection()->id() == SectionId::OnlineSingle) {
        m_pageTitleText.setMessage(2033);
    } else {
        m_pageTitleText.setMessage(2034);
    }
}

void OnlineTopPage::onBack(u32 /* localPlayerId */) {
    m_replacement = PageId::None;
    push(PageId::WifiDisconnect, Anim::None);
}

void OnlineTopPage::onButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    m_instructionText.setMessage(4310 + button->m_index);
}

void OnlineTopPage::onWorldwideButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto globalContext = SectionManager::Instance()->globalContext();
    globalContext->m_currentPack = 0;

    m_replacement = PageId::OnlineModeSelect;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineTopPage::onTrackpackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    if (SP::TrackPackManager::Instance().getPackCount() == 1) {
        auto *section = SectionManager::Instance()->currentSection();
        auto messagePage = section->page<PageId::MessagePopup>();

        messagePage->reset();
        messagePage->setWindowMessage(20047);
        return push(PageId::MessagePopup, Anim::None);
    }

    m_replacement = PageId::PackSelect;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineTopPage::showUnimplemented() {
    auto section = SectionManager::Instance()->currentSection();
    auto messagePopup = section->page<PageId::MessagePopup>();

    messagePopup->reset();
    messagePopup->setWindowMessage(20046);

    push(PageId::MessagePopup, Anim::None);
}

void OnlineTopPage::onFriendButtonFront(PushButton * /* button */, u32 /* localPlayerId */) {
    showUnimplemented();
}

void OnlineTopPage::onDirectButtonFront(PushButton * /* button */, u32 /* localPlayerId */) {
    auto section = SectionManager::Instance()->currentSection();
    auto confirmPopup = section->page<PageId::YesNoPopup>();

    confirmPopup->reset();
    confirmPopup->setWindowMessage(20041);
    confirmPopup->configureButton(0, 20042, nullptr, Anim::Next, &m_onDirectConfirm);
    confirmPopup->configureButton(1, 20043, nullptr, Anim::Next, &m_onDirectConfirm);

    push(PageId::YesNoPopup, Anim::Next);
}

void OnlineTopPage::onBackButtonFront(PushButton * /* button */, u32 localPlayerId) {
    onBack(localPlayerId);
}

void OnlineTopPage::onDirectConfirm(s32 choice, PushButton *button) {
    if (choice == 0) {
        m_replacement = PageId::DirectConnection;
        startReplace(Anim::Next, button->getDelay());
    }
}

} // namespace UI
