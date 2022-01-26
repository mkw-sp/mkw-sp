#pragma once

#include "GhostManagerPage.h"
#include "GhostSelectControl.h"
#include "Page.h"
#include "SheetSelectControl.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    LayoutUIControl switchLabel;
    GhostSelectControl ghostSelects[2];
    SheetSelectControl sheetSelect;
    LayoutUIControl sheetLabel;
    PushButton launchButton;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    InputHandler onOption;
    SheetSelectControlHandler onSheetSelectRight;
    SheetSelectControlHandler onSheetSelectLeft;
    PushButtonHandler onLaunchButtonSelect;
    PushButtonHandler onLaunchButtonFront;
    PushButtonHandler onBackButtonFront;
    GhostSelectControl *shownGhostSelect;
    GhostSelectControl *hiddenGhostSelect;
    bool isReplay;
    GhostList *ghostList;
    u32 chosenCount;
    bool ghostIsChosen[GHOST_TYPE_MAX];
    u32 sheetCount;
    u32 sheetIndex;
    s32 lastSelected;
    s32 replacement;
} TimeAttackGhostListPage;

TimeAttackGhostListPage *TimeAttackGhostListPage_ct(TimeAttackGhostListPage *this);

void TimeAttackGhostListPage_chooseGhost(TimeAttackGhostListPage *this, u32 buttonIndex);
