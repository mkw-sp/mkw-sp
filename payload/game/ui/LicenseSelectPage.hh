#pragma once

extern "C" {
#include "LicenseSelectPage.h"
}

#include "game/system/SaveManager.hh"
#include "game/ui/ConfirmPage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <array>

namespace UI {

class LicenseSelectPage : public Page {
public:
    LicenseSelectPage();
    ~LicenseSelectPage() override;
    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onServicePackButtonFront(PushButton *button, u32 localPlayerId);
    void onLicenseButtonFront(PushButton *button, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onCreateConfirm(ConfirmPage *page, f32 delay);
    void onChangeConfirm(ConfirmPage *page, f32 delay);
    void onCancel(ConfirmPage *page, f32 delay);

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_servicePackButton;
    CtrlMenuBackButton m_backButton;
    std::array<PushButton, 6> m_licenseButtons;
    MultiControlInputManager::Handler<LicenseSelectPage> m_onBack;
    PushButton::Handler<LicenseSelectPage> m_onServicePackButtonFront;
    PushButton::Handler<LicenseSelectPage> m_onLicenseButtonFront;
    PushButton::Handler<LicenseSelectPage> m_onBackButtonFront;
    ConfirmPage::Handler<LicenseSelectPage> m_onCreateConfirm;
    ConfirmPage::Handler<LicenseSelectPage> m_onChangeConfirm;
    ConfirmPage::Handler<LicenseSelectPage> m_onCancel;
    PageId m_replacement;
    MiiGroup m_miiGroup;
};

} // namespace UI
