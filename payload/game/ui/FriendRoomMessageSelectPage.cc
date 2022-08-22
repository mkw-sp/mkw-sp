#include "FriendRoomMessageSelectPage.hh"

#include "game/ui/MenuInputManager.hh"

namespace UI {

FriendRoomMessageSelectPage::FriendRoomMessageSelectPage() = default;

FriendRoomMessageSelectPage::~FriendRoomMessageSelectPage() = default;

void FriendRoomMessageSelectPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);
    initChildren(7);

    insertChild(0, &m_commentSelectBG, 0);
    insertChild(1, &m_messageSelects[0], 0);
    insertChild(2, &m_messageSelects[1], 0);
    insertChild(3, &m_sheetSelect, 0);
    insertChild(4, &m_messageSelectPageNum, 0);
    insertChild(5, &m_friendRoomMessageSelectObiBottom, 0);
    insertChild(6, &m_backButton, 0);

    m_commentSelectBG.load("control", "CommentSelectBG", "CommentSelectBG", nullptr);
    m_messageSelects[0].load();
    m_messageSelects[1].load();
    m_sheetSelect.load("button", "CommentSelectArrowRight", "ButtonArrowRight", "CommentSelectArrowLeft", "ButtonArrowLeft", 1, false, false);
    m_messageSelectPageNum.load("control", "MessageSelectPageNum", "MessageSelectPageNum", nullptr);
    m_friendRoomMessageSelectObiBottom.load("bg", "FriendRoomMessageSelectObiBottom", "MenuObyBottom", nullptr);
    m_backButton.load("button", "FriendRoomMessageSelectBack", "ButtonBack", 1, false, true);

    m_sheetSelect.setRightHandler(nullptr);
    m_sheetSelect.setLeftHandler(nullptr);
    m_backButton.setFrontHandler(nullptr, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.selectDefault(0);
}

void FriendRoomMessageSelectPage::onActivate() {
    switch (m_menuType) {
    case MenuType::Comment:
        m_messageCount = 96;

        m_commentSelectBG.setPaneVisible("blue_null", true);
        m_commentSelectBG.setPaneVisible("yellow_null", false);
        m_commentSelectBG.setPaneVisible("pink_null", false);
        break;
    case MenuType::Close:
        m_messageCount = 4;
        break;
    case MenuType::Register:
        // m_messageCount should be set to non-friends in room
        // Requires online implementation
        break;
    default:
        break;
    }

    // Vanilla accounts for negative numbers, this doesn't
    m_maxPageIdx = m_messageCount / 4 + 1;
    m_currentPageIdx = 0;
}

void FriendRoomMessageSelectPage::onDeactivate() {
    m_menuType = MenuType::None;
}

void FriendRoomMessageSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {
    startReplace(Anim::Prev, 0.0f);
}

} // namespace UI
