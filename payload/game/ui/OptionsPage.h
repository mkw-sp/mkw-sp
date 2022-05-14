#pragma once

#include "Page.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    PushButton settingsButton;
    PushButton tracksButton;
    PushButton updatesButton;
    PushButton aboutButton;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    PushButtonHandler onBackButtonFront;
} OptionsPage;
