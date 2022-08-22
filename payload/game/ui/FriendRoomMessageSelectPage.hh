#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/Page.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"

namespace UI {

class FriendRoomMessageSelectPage : public Page {
public:
    enum class MenuType {
        None = -1,

        Comment = 0,
        Close = 1,
        Register = 2,
    };

    FriendRoomMessageSelectPage();
    ~FriendRoomMessageSelectPage() override;

    void onInit() override;
    void onActivate() override;
    void onDeactivate() override;

private:
    class MessageSelectControl : public LayoutUIControl {
    public:
        MessageSelectControl() = default;
        ~MessageSelectControl() override = default;
        void load();
    private:
        PushButton m_buttons[4];
    };

    void onCommentButtonFront(PushButton *button, u32 localPlayerId);
    void onCloseButtonFront(PushButton *button, u32 localPlayerId);
    void onRegisterButtonFront();
    void onRight();
    void onLeft();
    void onBackButtonFront();
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<FriendRoomMessageSelectPage>;

    MultiControlInputManager m_inputManager;
    LayoutUIControlScaleFade m_commentSelectBG;
    MessageSelectControl m_messageSelects[2];
    SheetSelectControlScaleFade m_sheetSelect;
    LayoutUIControlScaleFade m_messageSelectPageNum;
    LayoutUIControl m_friendRoomMessageSelectObiBottom;
    CtrlMenuBackButton m_backButton;
    MenuType m_menuType;
    s32 m_messageCount;
    s32 m_maxPageIdx;
    s32 m_currentPageIdx;

    H<MultiControlInputManager> m_onBack{ this, &FriendRoomMessageSelectPage::onBack };
    /*H<PushButton> m_onCommentButtonFront{ this, &FriendRoomMessageSelectPage::onCommentButtonFront };
    H<PushButton> m_onCloseButtonFront{ this, &FriendRoomMessageSelectPage::onCloseButtonFront };*/
};

} // namespace UI
