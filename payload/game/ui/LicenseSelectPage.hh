#pragma once

#include "game/ui/ConfirmPage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <array>

namespace UI {

class LicenseSelectPage : public Page {
public:
    LicenseSelectPage();
    LicenseSelectPage(const LicenseSelectPage &) = delete;
    LicenseSelectPage(LicenseSelectPage &&) = delete;
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

    template <typename T>
    using H = typename T::Handler<LicenseSelectPage>;

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    PushButton m_servicePackButton;
    CtrlMenuBackButton m_backButton;
    std::array<PushButton, 6> m_licenseButtons;
    H<MultiControlInputManager> m_onBack{this, &LicenseSelectPage::onBack};
    H<PushButton> m_onServicePackButtonFront{this, &LicenseSelectPage::onServicePackButtonFront};
    H<PushButton> m_onLicenseButtonFront{this, &LicenseSelectPage::onLicenseButtonFront};
    H<PushButton> m_onBackButtonFront{this, &LicenseSelectPage::onBackButtonFront};
    H<ConfirmPage> m_onCreateConfirm{this, &LicenseSelectPage::onCreateConfirm};
    H<ConfirmPage> m_onChangeConfirm{this, &LicenseSelectPage::onChangeConfirm};
    H<ConfirmPage> m_onCancel{this, &LicenseSelectPage::onCancel};
    PageId m_replacement;
    MiiGroup m_miiGroup;
};

} // namespace UI
