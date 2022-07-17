#pragma once

#include "game/ui/GhostManagerPage.hh"
#include "game/ui/GhostSelectControl.hh"
#include "game/ui/MessageWindowControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/settings/ClientSettings.hh>

namespace UI {

class TimeAttackGhostListPage : public Page {
    friend class GhostSelectControl;
    friend class GhostSelectButton;
    friend class TimeAttackModeSelectPage;
public:
    TimeAttackGhostListPage();
    TimeAttackGhostListPage(const TimeAttackGhostListPage &) = delete;
    TimeAttackGhostListPage(TimeAttackGhostListPage &&) = delete;
    ~TimeAttackGhostListPage();

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    void chooseGhost(u32 buttonIndex);

private:
    void onBack(u32 localPlayerId);
    void onSettingsButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsButtonSelect(PushButton *button, u32 localPlayerId);
    void onSettingsButtonDeselect(PushButton *button, u32 localPlayerId);
    void onSheetSelectRight(SheetSelectControl *control, u32 localPlayerId);
    void onSheetSelectLeft(SheetSelectControl *control, u32 localPlayerId);
    void onOKButtonFront(PushButton *button, u32 localPlayerId);
    void onOKButtonSelect(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void refreshSheetLabel();
    bool canSwapGhostSelects() const;
    void swapGhostSelects();
    void refreshOKButton();

    template <typename T>
    using H = typename T::Handler<TimeAttackGhostListPage>;

    // UI elements
    MultiControlInputManager m_input;
    CtrlMenuPageTitleText m_titleText;
    PushButton m_settingsButton;
    std::array<GhostSelectControl, 2> m_ghostSelects;
    SheetSelectControl m_sheetSelect;
    LayoutUIControl m_sheetLabel;
    MessageWindowControl m_messageWindow;
    PushButton m_okButton;
    CtrlMenuBackButton m_backButton;

    // Input handlers
    H<MultiControlInputManager> m_onBack{ this, &TimeAttackGhostListPage::onBack };
    H<PushButton> m_onSettingsButtonSelect{ this, &TimeAttackGhostListPage::onSettingsButtonSelect };
    H<PushButton> m_onSettingsButtonDeselect{ this, &TimeAttackGhostListPage::onSettingsButtonDeselect };
    H<PushButton> m_onSettingsButtonFront{ this, &TimeAttackGhostListPage::onSettingsButtonFront };
    H<SheetSelectControl> m_onSheetSelectRight{ this, &TimeAttackGhostListPage::onSheetSelectRight };
    H<SheetSelectControl> m_onSheetSelectLeft{ this, &TimeAttackGhostListPage::onSheetSelectLeft };
    H<PushButton> m_onOKButtonSelect{ this, &TimeAttackGhostListPage::onOKButtonSelect };
    H<PushButton> m_onOKButtonFront{ this, &TimeAttackGhostListPage::onOKButtonFront };
    H<PushButton> m_onBackButtonFront{ this, &TimeAttackGhostListPage::onBackButtonFront };

    GhostSelectControl *m_shownGhostSelect;
    GhostSelectControl *m_hiddenGhostSelect;
    const GhostManagerPage::SPList *m_ghostList;
    u32 m_chosenCount;
    std::array<bool, System::MAX_GHOST_COUNT> m_ghostIsChosen;
    u32 m_sheetCount;
    u32 m_sheetIndex;
    s32 m_lastSelected;
    std::optional<SP::ClientSettings::TAClass> m_cc{};
    PageId m_replacement;
};

} // namespace UI
