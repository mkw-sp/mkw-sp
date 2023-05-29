#include "FriendRoomRulesPage.hh"

namespace UI {

FriendRoomRulesPage::FriendRoomRulesPage() = default;

FriendRoomRulesPage::~FriendRoomRulesPage() = default;

void FriendRoomRulesPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(4);
    insertChild(0, &m_rules, 0);
    insertChild(1, &m_messageWindow, 0);
    insertChild(2, &m_okKey, 0);
    insertChild(3, &m_blackBack, 0);

    m_rules.load("message_window", "RoomRules", "RoomRules", nullptr);
    m_messageWindow.load("message_window", "MessageWindow", "MessageWindowPopup");
    m_okKey.load("message_window", "OKKey", "OKKeyPopup", nullptr);
    m_blackBack.load("message_window", "BlackBack", "BlackBack");
    m_blackBack.m_zIndex = -1.0f;

    m_inputManager.setHandler(MenuInputManager::InputId::Front, &m_onFront, false);

    for (size_t i = 0; i < SP::RoomSettings::count; i++) {
        const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
        char namePane[0x20];
        snprintf(namePane, std::size(namePane), "name_%u", i);
        m_rules.setMessage(namePane, entry.messageId);
        char valuePane[0x20];
        snprintf(valuePane, std::size(valuePane), "value_%u", i);
        if (entry.valueNames) {
            m_rules.setMessage(valuePane, entry.valueMessageIds[entry.defaultValue]);
        } else {
            MessageInfo info{};
            info.intVals[0] = entry.defaultValue;
            m_rules.setMessage(valuePane, entry.valueMessageIds[0], &info);
        }
    }

    setAnimSfxIds(Sound::SoundId::SE_UI_PAGE_NEXT, Sound::SoundId::SE_DUMMY);

    m_popRequested = false;
}

void FriendRoomRulesPage::beforeInAnim() {
    playSound(Sound::SoundId::SE_UI_CTRL_WIN_POPUP, -1);
}

void FriendRoomRulesPage::afterOutAnim() {
    playSound(Sound::SoundId::SE_UI_CTRL_WIN_POPDOWN, -1);
}

void FriendRoomRulesPage::beforeCalc() {
    if (state() == State::State4 && m_popRequested) {
        startReplace(Anim::Prev, 0.0f);
        m_popRequested = false;
    }
}

void FriendRoomRulesPage::afterCalc() {
    MessageInfo info{};
    info.playerIds[0] = 0;
    m_okKey.setMessageAll(9504, &info);
}

void FriendRoomRulesPage::pop(Anim anim) {
    setAnim(anim);
    m_popRequested = true;
}

void FriendRoomRulesPage::refresh(const std::array<u32, SP::RoomSettings::count> &settings) {
    for (size_t i = 0; i < settings.size(); i++) {
        const auto &entry = SP::ClientSettings::entries[SP::RoomSettings::offset + i];
        char valuePane[0x20];
        snprintf(valuePane, std::size(valuePane), "value_%u", i);
        if (entry.valueNames) {
            m_rules.setMessage(valuePane, entry.valueMessageIds[settings[i]]);
        } else {
            MessageInfo info{};
            info.intVals[0] = settings[i];
            m_rules.setMessage(valuePane, entry.valueMessageIds[0], &info);
        }
    }
}

void FriendRoomRulesPage::onFront(u32 /* localPlayerId */) {
    startReplace(Anim::None, 0.0f);
}

} // namespace UI
