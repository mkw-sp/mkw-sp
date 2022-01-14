#pragma once

#include "LicenseSelectButton.h"
#include "Page.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    CtrlMenuBackButton backButton;
    LicenseSelectButton licenseButtons[6];
    InputHandler onBack;
    s32 replacement;
} LicenseSelectPage;

LicenseSelectPage *LicenseSelectPage_ct(LicenseSelectPage *this);
