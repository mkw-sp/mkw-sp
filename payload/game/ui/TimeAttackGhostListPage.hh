#pragma once

#include "game/ui/GhostManagerPage.hh"
#include "game/ui/GhostSelectControl.hh"
#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class TimeAttackGhostListPage : public Page {
    friend class GhostSelectControl;
    friend class GhostSelectButton;
public:
    TimeAttackGhostListPage();
    TimeAttackGhostListPage(const TimeAttackGhostListPage &) = delete;
    TimeAttackGhostListPage(TimeAttackGhostListPage &&) = delete;
    ~TimeAttackGhostListPage();

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void onRefocus() override;

    void chooseGhost(u32 buttonIndex);

private:
    void onBack(u32 localPlayerId);
    void refreshLaunchButton();
    void onOption(u32 localPlayerId);
    bool canSwapGhostSelects() const;
    void refreshSheetLabel();
    void swapGhostSelects();
    void onSheetSelectRight(SheetSelectControl *control, u32 localPlayerId);
    void onSheetSelectLeft(SheetSelectControl *control, u32 localPlayerId);
    void onLaunchButtonFront(PushButton *button, u32 localPlayerId);
    void onLaunchButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::Handler<TimeAttackGhostListPage>;

    // UI elements
    MultiControlInputManager m_input;
    CtrlMenuPageTitleText m_titleText;
    LayoutUIControl m_switchLabel;
    std::array<GhostSelectControl, 2> m_ghostSelects;
    SheetSelectControl m_sheetSelect;
    LayoutUIControl m_sheetLabel;
    MessageWindowControl m_messageWindow;
    PushButton m_launchButton;
    CtrlMenuBackButton m_backButton;

    // Input handlers
    H<MultiControlInputManager> m_onBack{ this, &TimeAttackGhostListPage::onBack };
    H<MultiControlInputManager> m_onOption{ this, &TimeAttackGhostListPage::onOption };
    H<SheetSelectControl> m_onSheetSelectRight{ this, &TimeAttackGhostListPage::onSheetSelectLeft };
    H<SheetSelectControl> m_onSheetSelectLeft{ this, &TimeAttackGhostListPage::onSheetSelectRight };
    H<PushButton> m_onLaunchButtonSelect{ this, &TimeAttackGhostListPage::onLaunchButtonSelect };
    H<PushButton> m_onLaunchButtonFront{ this, &TimeAttackGhostListPage::onLaunchButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &TimeAttackGhostListPage::onBackButtonFront };

    GhostSelectControl *m_shownGhostSelect;
    GhostSelectControl *m_hiddenGhostSelect;
    bool m_isReplay;
    const GhostManagerPage::SPList *m_ghostList;
    u32 m_chosenCount;
    std::array<bool, System::MAX_GHOST_COUNT> m_ghostIsChosen;
    u32 m_sheetCount;
    u32 m_sheetIndex;
    s32 m_lastSelected;
    PageId m_replacement;
};

} // namespace UI
