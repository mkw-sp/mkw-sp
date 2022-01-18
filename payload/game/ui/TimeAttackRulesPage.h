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
    RadioButtonControl ruleControls[3];
    PushButton okButton;
    CtrlMenuInstructionText instructionText;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    RadioButtonControlHandler onRuleControlFront;
    RadioButtonControlHandler onRuleControlSelect;
    InputHandler onOkButtonFront;
    InputHandler onOkButtonSelect;
    s32 replacement;
} TimeAttackRulesPage;
