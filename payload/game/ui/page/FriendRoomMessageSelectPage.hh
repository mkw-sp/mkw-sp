#pragma once

#include "game/ui/MenuInputManager.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/SheetSelectControl.hh"

namespace UI {

class FriendRoomMessageSelectPage : public Page {
public:
    enum class MenuType {
        None = -1,

        Comment = 0,
        Start = 1,
        Register = 2,
    };

    FriendRoomMessageSelectPage();
    ~FriendRoomMessageSelectPage() override;

    void onInit() override;
    void onActivate() override;
    void onDeactivate() override;
    void beforeCalc() override;

    void setMenuType(MenuType menuType) {
        m_menuType = menuType;
    }
    void pop(Anim anim);

private:
    class MessageSelectControl : public LayoutUIControl {
    public:
        MessageSelectControl();
        ~MessageSelectControl() override;
        void calcSelf() override;
        void vf_20() override;
        void vf_28() override;
        void vf_2c() override;

        void load();
        // Set animations
        void show();
        void hide();
        void slideIn(bool isLeft);
        void slideOut(bool isLeft);
        // Get animations
        bool isShown();
        bool isHidden();

        PushButton m_buttons[4];
    };

    void refresh();

    void onCommentButtonFront(PushButton *button, u32 localPlayerId);
    void onStartButtonFront(PushButton *button, u32 localPlayerId);
    void onRegisterButtonFront();
    void onRight(SheetSelectControl *control, u32 localPlayerId);
    void onLeft(SheetSelectControl *control, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
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
    MessageSelectControl *m_visibleMessageSelect;
    MessageSelectControl *m_hiddenMessageSelect;
    bool m_popRequested;
    MenuType m_menuType;
    s32 m_messageCount;
    s32 m_sheetCount;
    s32 m_currentSheetIdx;
    s32 m_cachedSheetIdx;
    s32 m_cachedButton;

    H<MultiControlInputManager> m_onBack{this, &FriendRoomMessageSelectPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &FriendRoomMessageSelectPage::onBackButtonFront};
    H<PushButton> m_onCommentButtonFront{this, &FriendRoomMessageSelectPage::onCommentButtonFront};
    H<SheetSelectControl> m_onRight{this, &FriendRoomMessageSelectPage::onRight};
    H<SheetSelectControl> m_onLeft{this, &FriendRoomMessageSelectPage::onLeft};
    H<PushButton> m_onStartButtonFront{this, &FriendRoomMessageSelectPage::onStartButtonFront};
};

} // namespace UI
