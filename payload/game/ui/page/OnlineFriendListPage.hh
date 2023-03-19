#pragma once

#include "game/ui/FriendButton.hh"
#include "game/ui/Page.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class OnlineFriendListPage : public Page {
public:
    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    // Gets the friend list from OnlineConnectionManagerPage
    // and refreshes the UI.
    void refresh();

    size_t m_selectedFriend = 0;

private:
    void onFriendListLeft(SheetSelectControl *sheet, u32 localPlayerId);
    void onFriendListRight(SheetSelectControl *sheet, u32 localPlayerId);
    void onFriendFront(PushButton *button, u32 localPlayerId);
    void onFriendSelect(PushButton *button, u32 localPlayerId);
    void onBackFront(PushButton *button, u32 localPlayerId);
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<OnlineFriendListPage>;

    FriendListButton m_friendButtons[5];
    CtrlMenuPageTitleText m_titleText;
    SheetSelectControl m_friendList;
    CtrlMenuBackButton m_backButton;
    LayoutUIControl m_pageNum;

    H<PushButton> m_onFriendSelect{this, &OnlineFriendListPage::onFriendSelect};
    H<PushButton> m_onFriendFront{this, &OnlineFriendListPage::onFriendFront};
    H<PushButton> m_onBackFront{this, &OnlineFriendListPage::onBackFront};
    H<SheetSelectControl> m_onFriendListLeft{this, &OnlineFriendListPage::onFriendListLeft};
    H<SheetSelectControl> m_onFriendListRight{this, &OnlineFriendListPage::onFriendListRight};
    H<MultiControlInputManager> m_onBack{this, &OnlineFriendListPage::onBack};

    MiiGroup m_miiGroup;
    PageId m_replacement;
    MultiControlInputManager m_inputManager;
};

} // namespace UI
