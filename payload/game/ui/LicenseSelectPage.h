#pragma once

#include "ConfirmPage.h"
#include "LicenseSelectButton.h"
#include "Page.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    PushButton aboutButton;
    CtrlMenuBackButton backButton;
    LicenseSelectButton licenseButtons[6];
    InputHandler onBack;
    PushButtonHandler onAboutButtonFront;
    ConfirmPageHandler onAboutConfirm;
    PushButtonHandler onBackButtonFront;
    s32 replacement;
} LicenseSelectPage;

LicenseSelectPage *LicenseSelectPage_ct(LicenseSelectPage *this);
