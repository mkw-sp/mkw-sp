#include "OnlineTopPage.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

namespace UI {

enum ButtonId {
    Worldwide,
    Trackpack,
    Friend,
};

OnlineTopPage::OnlineTopPage() = default;

OnlineTopPage::~OnlineTopPage() = default;

PageId OnlineTopPage::getReplacement() {
    return m_replacement;
}

static const char *animInfo[] = {
    "State", "Offline", "RandomMatching", "FriendParent", nullptr,
    "State2", "Offline2", "RandomMatching2", "FriendParent2", nullptr,
    nullptr
};

void OnlineTopPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false);

    initChildren(6);
    insertChild(0, &m_worldwideButton, 0);
    insertChild(1, &m_trackpackButton, 0);
    insertChild(2, &m_friendButton, 0);
    insertChild(3, &m_backButton, 0);
    insertChild(4, &m_pageTitleText, 0);
    insertChild(5, &m_instructionText, 0);

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

    m_backButton.load("button", "Back", "ButtonBack", 1, 0, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.load(false);
    m_instructionText.load();
}

void OnlineTopPage::onActivate() {
    m_replacement = PageId::None;
    m_worldwideButton.selectDefault(0);
    m_instructionText.setMessage(0x10d6);

    if (SectionManager::Instance()->currentSection()->id() == SectionId::OnlineSingle) {
        m_pageTitleText.setMessage(0x7f1);
    } else {
        m_pageTitleText.setMessage(0x7f2);
    }
}

void OnlineTopPage::onBack(u32 localPlayerId) {
    m_replacement = PageId::None;
    push(PageId::WifiDisconnect, Anim::None);
}

void OnlineTopPage::onButtonSelect(PushButton* button, [[maybe_unused]] u32 localPlayerId) {
    m_instructionText.setMessage(0x10d6 + button->m_index);
}

void OnlineTopPage::onWorldwideButtonFront(PushButton* button, [[maybe_unused]] u32 localPlayerId) {
    auto section = SectionManager::Instance()->currentSection();
    auto connectionManager = section->page<PageId::OnlineConnectionManager>();
    connectionManager->setTrackpack(0);

    m_replacement = PageId::WifiModeSelect;
    startReplace(Anim::Next, button->getDelay());
}

void OnlineTopPage::onTrackpackButtonFront(PushButton* button, [[maybe_unused]] u32 localPlayerId) {
    SP_LOG("OnlineTopPage::onTrackpackButtonFront");
}

void OnlineTopPage::onFriendButtonFront(PushButton* button, [[maybe_unused]] u32 localPlayerId) {
    SP_LOG("OnlineTopPage::onFriendButtonFront");
}

void OnlineTopPage::onBackButtonFront([[maybe_unused]] PushButton* button, u32 localPlayerId) {
    onBack(localPlayerId);
}

} // namespace UI
