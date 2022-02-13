#pragma once

#include "Page.h"
#include "RadioButtonControl.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuInstructionText.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    RadioButtonControl settingControls[5];
    CtrlMenuInstructionText instructionText;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    RadioButtonControlHandler onSettingControlFront;
    RadioButtonControlHandler onSettingControlSelect;
    PushButtonHandler onBackButtonFront;
    bool resetSelection;

    // Instruction text & back button not available during race
    bool enableInstructionText;
    bool enableBackButton;
    s32 replacementPage;
} LicenseSettingsPage;
