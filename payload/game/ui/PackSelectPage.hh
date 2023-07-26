#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ScrollBar.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class PackSelectPage : public Page {
public:
    PackSelectPage();
    PackSelectPage(const PackSelectPage &) = delete;
    PackSelectPage(PackSelectPage &&) = delete;
    ~PackSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    static std::optional<u32> s_lastPackFront;

private:
    void onBack(u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onSheetSelectRight(SheetSelectControl *control, u32 localPlayerId);
    void onSheetSelectLeft(SheetSelectControl *control, u32 localPlayerId);
    void onScrollBarChange(ScrollBar *scrollBar, u32 localPlayerId, u32 chosen);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    void refresh();

    template <typename T>
    using H = typename T::Handler<PackSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    std::array<PushButton, 3> m_buttons;
    SheetSelectControl m_sheetSelect;
    LayoutUIControl m_sheetLabel;
    ScrollBar m_scrollBar;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &PackSelectPage::onBack};
    H<PushButton> m_onButtonFront{this, &PackSelectPage::onButtonFront};
    H<PushButton> m_onButtonSelect{this, &PackSelectPage::onButtonSelect};
    H<SheetSelectControl> m_onSheetSelectRight{this, &PackSelectPage::onSheetSelectRight};
    H<SheetSelectControl> m_onSheetSelectLeft{this, &PackSelectPage::onSheetSelectLeft};
    H<ScrollBar> m_onScrollBarChange{this, &PackSelectPage::onScrollBarChange};
    H<PushButton> m_onBackButtonFront{this, &PackSelectPage::onBackButtonFront};
    PageId m_replacement;
    u32 m_sheetCount;
    u32 m_sheetIndex;
    u32 m_lastSelected;
};

} // namespace UI
