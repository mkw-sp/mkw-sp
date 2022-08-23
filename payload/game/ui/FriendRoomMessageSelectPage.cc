#include "FriendRoomMessageSelectPage.hh"

#include "game/ui/Font.hh"
#include "game/ui/MenuInputManager.hh"
#include "sp/cs/RoomClient.hh"

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

    m_sheetSelect.setRightHandler(&m_onRight);
    m_sheetSelect.setLeftHandler(&m_onLeft);
    m_backButton.setFrontHandler(nullptr, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.selectDefault(0);

    m_cachedSheetIdx = 0;
    m_cachedButton = 0;
}

void FriendRoomMessageSelectPage::onActivate() {
    m_visibleMessageSelect = &m_messageSelects[0];
    m_hiddenMessageSelect = &m_messageSelects[1];

    switch (m_menuType) {
    case MenuType::Comment:
        m_messageCount = 96;

        for (u8 i = 0; i < 4; i++) {
            m_visibleMessageSelect->m_buttons[i].setFrontHandler(&m_onCommentButtonFront, false);
            m_hiddenMessageSelect->m_buttons[i].setFrontHandler(&m_onCommentButtonFront, false);
        }

        m_commentSelectBG.setPaneVisible("blue_null", true);
        m_commentSelectBG.setPaneVisible("yellow_null", false);
        m_commentSelectBG.setPaneVisible("pink_null", false);
        break;
    case MenuType::Close:
        m_messageCount = 4;

        m_commentSelectBG.setPaneVisible("blue_null", false);
        m_commentSelectBG.setPaneVisible("yellow_null", false);
        m_commentSelectBG.setPaneVisible("pink_null", true);
        break;
    case MenuType::Register:
        // m_messageCount should be set to non-friends in room
        // Requires online implementation

        m_commentSelectBG.setPaneVisible("blue_null", false);
        m_commentSelectBG.setPaneVisible("yellow_null", true);
        m_commentSelectBG.setPaneVisible("pink_null", false);
        break;
    default:
        break;
    }

    // Vanilla accounts for negative numbers, this doesn't
    m_pageCount = (m_messageCount + 3) / 4;
    m_currentPageIdx = m_menuType == MenuType::Comment ? m_cachedSheetIdx : 0;
    m_sheetSelect.configure(m_pageCount > 1, m_pageCount > 1);
    initText();

    m_visibleMessageSelect->show();
    m_hiddenMessageSelect->hide();

    if (m_messageCount < 0) { return; }
    if (m_menuType == MenuType::Comment) {
        m_visibleMessageSelect->m_buttons[m_cachedButton].selectDefault(0);
    } else {
        m_visibleMessageSelect->m_buttons[0].selectDefault(0);
    }
}

void FriendRoomMessageSelectPage::onDeactivate() {
    m_menuType = MenuType::None;
}

void FriendRoomMessageSelectPage::initText() {
    for (s8 i = 0; i < 4; i++) {
        s32 currentButton = i + m_currentPageIdx * 4;
        if (currentButton < m_messageCount) {
            MessageInfo messageInfo;
            s32 messageId = 0;
            switch (m_menuType) {
            case MenuType::Comment:
                messageId = currentButton + 4500;
                break;
            case MenuType::Close:
                messageId = currentButton + 4110;
                break;
            case MenuType::Register:
            default:
                break;
            }
            m_visibleMessageSelect->m_buttons[i].m_index = i;
            m_visibleMessageSelect->m_buttons[i].setVisible(i >= 0);
            m_visibleMessageSelect->m_buttons[i].setMessageAll(messageId, &messageInfo);
        } else {
            m_visibleMessageSelect->m_buttons[i].m_index = -1;
            m_visibleMessageSelect->m_buttons[i].setVisible(false);
        }
    }

    if (m_menuType == MenuType::Comment) {
        MessageInfo messageInfo;
        messageInfo.intVals[0] = m_currentPageIdx + 1;
        messageInfo.intVals[1] = m_pageCount;
        m_messageSelectPageNum.setMessageAll(2009, &messageInfo);
    } else {
        m_messageSelectPageNum.setMessageAll(0, nullptr);
    }
}

void FriendRoomMessageSelectPage::onBack([[maybe_unused]] u32 localPlayerId) {
    startReplace(Anim::Prev, 0.0f);
}

void FriendRoomMessageSelectPage::onCommentButtonFront(PushButton *button, [[maybe_unused]] u32 localPlayerId) {
    u32 messageId = button->m_index + m_currentPageIdx * 4;
    SP::RoomClient::Instance()->sendComment(messageId);
    startReplace(Anim::Next, button->getDelay());
}

void FriendRoomMessageSelectPage::onRight([[maybe_unused]] SheetSelectControl *control, [[maybe_unused]] u32 localPlayerId) {
    if (!m_visibleMessageSelect->isShown() || !m_hiddenMessageSelect->isHidden()) { return; }

    m_visibleMessageSelect->slideOut(true);
    m_currentPageIdx++;
    if (m_currentPageIdx >= m_pageCount) { m_currentPageIdx = 0; }

    std::swap(m_visibleMessageSelect, m_hiddenMessageSelect);

    initText();
    m_visibleMessageSelect->slideIn(true);
}

void FriendRoomMessageSelectPage::onLeft([[maybe_unused]] SheetSelectControl *control, [[maybe_unused]] u32 localPlayerId) {
    if (!m_visibleMessageSelect->isShown() || !m_hiddenMessageSelect->isHidden()) { return; }

    m_visibleMessageSelect->slideOut(false);
    m_currentPageIdx--;
    if (m_currentPageIdx < 0) { m_currentPageIdx = m_pageCount - 1; }

    std::swap(m_visibleMessageSelect, m_hiddenMessageSelect);

    initText();
    m_visibleMessageSelect->slideIn(false);
}

FriendRoomMessageSelectPage::MessageSelectControl::MessageSelectControl() = default;

FriendRoomMessageSelectPage::MessageSelectControl::~MessageSelectControl() = default;

} // namespace UI
