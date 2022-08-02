#pragma once

#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class SettingsPage : public Page {
public:
    SettingsPage();
    SettingsPage(const SettingsPage &) = delete;
    SettingsPage(SettingsPage &&) = delete;
    ~SettingsPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onDeinit() override;
    void onActivate() override;

    void setReplacement(PageId pageId);

private:
    struct CategoryInfo {
        u32 categoryIndex;
        u32 categorySheetIndex;
        u32 categorySheetCount;
        u32 settingIndex;
    };

    void onBack(u32 localPlayerId);
    void onCategoryControlFront(UpDownControl *control, u32 localPlayerId);
    void onCategoryControlSelect(UpDownControl *control, u32 localPlayerId);
    void onCategoryValueChange(TextUpDownValueControl::TextControl *text, u32 index);
    void onSettingControlChange(UpDownControl *control, u32 localPlayerId, u32 index);
    void onSettingControlFront(UpDownControl *control, u32 localPlayerId);
    void onSettingControlSelect(UpDownControl *control, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    u32 getSheetCount() const;
    CategoryInfo getCategoryInfo(u32 sheetIndex) const;

    template <typename T>
    using H = typename T::Handler<SettingsPage>;

    template <typename T>
    using C = typename T::ChangeHandler<SettingsPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    UpDownControl m_categoryControl;
    TextUpDownValueControl m_categoryValue;
    UpDownControl m_settingControls[6];
    TextUpDownValueControl m_settingValues[6];
    CtrlMenuInstructionText m_instructionText;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{ this, &SettingsPage::onBack };
    H<UpDownControl> m_onCategoryControlFront{ this, &SettingsPage::onCategoryControlFront };
    H<UpDownControl> m_onCategoryControlSelect{ this, &SettingsPage::onCategoryControlSelect };
    C<TextUpDownValueControl> m_onCategoryValueChange{ this, &SettingsPage::onCategoryValueChange };
    C<UpDownControl> m_onSettingControlChange { this, &SettingsPage::onSettingControlChange };
    H<UpDownControl> m_onSettingControlFront { this, &SettingsPage::onSettingControlFront };
    H<UpDownControl> m_onSettingControlSelect { this, &SettingsPage::onSettingControlSelect };
    H<PushButton> m_onBackButtonFront{ this, &SettingsPage::onBackButtonFront };

public:
    PageId m_replacement;
};

} // namespace UI
