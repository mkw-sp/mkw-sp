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
    RadioButtonControl ruleControls[6];
    PushButton okButton;
    CtrlMenuInstructionText instructionText;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    RadioButtonControlHandler onRuleControlFront;
    RadioButtonControlHandler onRuleControlSelect;
    PushButtonHandler onOkButtonFront;
    PushButtonHandler onOkButtonSelect;
    PushButtonHandler onBackButtonFront;
    s32 replacement;
} TimeAttackRulesPage;
