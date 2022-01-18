#include "TimeAttackGhostListPage.h"

#include "RaceConfirmPage.h"
#include "SectionManager.h"

#include "../system/RaceConfig.h"

#include <string.h>

static const Page_vt s_TimeAttackGhostListPage_vt;

static void onBack(InputHandler *this, u32 localPlayerId) {
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onBack);
    page->replacement = 0x6f; // TODO enum
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void TimeAttackGhostListPage_refreshLaunchButton(TimeAttackGhostListPage *this) {
    if (this->chosenCount == 0) {
        this->switchLabel.isHidden = true;
        LayoutUIControl_setMessageAll(&this->launchButton, 0x1780, NULL);
    } else if (this->chosenCount == 1) {
        this->switchLabel.isHidden = false;
        u32 messageId = this->isReplay ? 0x3001 : 0x3000;
        LayoutUIControl_setMessageAll(&this->launchButton, messageId, NULL);
    } else {
        u32 messageId = this->isReplay ? 0x3003 : 0x3002;
        ExtendedMessageInfo info = { .intVals[0] = this->chosenCount };
        LayoutUIControl_setMessageAll(&this->launchButton, messageId, &info);
    }
}

static void onOption(InputHandler *this, u32 localPlayerId) {
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onOption);
    if (!page->switchLabel.isHidden) {
        page->isReplay = !page->isReplay;
        TimeAttackGhostListPage_refreshLaunchButton(page);
    }
}

static const InputHandler_vt onOption_vt = {
    .handle = onOption,
};

static bool TimeAttackGhostListPage_canSwapGhostSelects(TimeAttackGhostListPage *this) {
    if (!GhostSelectControl_isShown(this->shownGhostSelect)) {
        return false;
    }

    if (!GhostSelectControl_isHidden(this->hiddenGhostSelect)) {
        return false;
    }

    return true;
}

static void TimeAttackGhostListPage_refreshSheetLabel(TimeAttackGhostListPage *this) {
    ExtendedMessageInfo info = {
        .intVals[0] = this->sheetIndex + 1,
        .intVals[1] = this->sheetCount,
    };
    LayoutUIControl_setMessageAll(&this->sheetLabel, 0x7d9, &info);
}

static void TimeAttackGhostListPage_swapGhostSelects(TimeAttackGhostListPage *this) {
    GhostSelectControl *tmp = this->shownGhostSelect;
    this->shownGhostSelect = this->hiddenGhostSelect;
    this->hiddenGhostSelect = tmp;
}

static void onSheetSelectRight(InputHandler *this, u32 localPlayerId) {
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onSheetSelectRight);
    if (!TimeAttackGhostListPage_canSwapGhostSelects(page)) {
        return;
    }

    if (page->sheetIndex == page->sheetCount - 1) {
        page->sheetIndex = 0;
    } else {
        page->sheetIndex++;
    }
    TimeAttackGhostListPage_refreshSheetLabel(page);

    GhostSelectControl_slideOutToLeft(page->shownGhostSelect);
    GhostSelectControl_slideInFromRight(page->hiddenGhostSelect);

    TimeAttackGhostListPage_swapGhostSelects(page);
}

static const InputHandler_vt onSheetSelectRight_vt = {
    .handle = onSheetSelectRight,
};

static void onSheetSelectLeft(InputHandler *this, u32 localPlayerId) {
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onSheetSelectLeft);
    if (!TimeAttackGhostListPage_canSwapGhostSelects(page)) {
        return;
    }

    if (page->sheetIndex == 0) {
        page->sheetIndex = page->sheetCount - 1;
    } else {
        page->sheetIndex--;
    }
    TimeAttackGhostListPage_refreshSheetLabel(page);

    GhostSelectControl_slideOutToRight(page->shownGhostSelect);
    GhostSelectControl_slideInFromLeft(page->hiddenGhostSelect);

    TimeAttackGhostListPage_swapGhostSelects(page);
}

static const InputHandler_vt onSheetSelectLeft_vt = {
    .handle = onSheetSelectLeft,
};

static void onLaunchButtonFront(PushButtonHandler *this, PushButton *button, u32 localPlayerId) {
    UNUSED(button);
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onLaunchButtonFront);
    page->vt->push(page, 0x4b, PAGE_ANIMATION_NEXT); // TODO enum
}

static const PushButtonHandler_vt onLaunchButtonFront_vt = {
    .handle = onLaunchButtonFront,
};

static void onLaunchButtonSelect(PushButtonHandler *this, PushButton *button, u32 localPlayerId) {
    UNUSED(button);
    UNUSED(localPlayerId);

    TimeAttackGhostListPage *page = container_of(this, TimeAttackGhostListPage, onLaunchButtonSelect);
    page->lastSelected = -1;
}

static const PushButtonHandler_vt onLaunchButtonSelect_vt = {
    .handle = onLaunchButtonSelect,
};

static TimeAttackGhostListPage *my_TimeAttackGhostListPage_ct(TimeAttackGhostListPage *this) {
    Page_ct(this);
    this->vt = &s_TimeAttackGhostListPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    LayoutUIControl_ct(&this->switchLabel);
    GhostSelectControl_ct(&this->ghostSelects[0]);
    GhostSelectControl_ct(&this->ghostSelects[1]);
    SheetSelectControl_ct(&this->sheetSelect);
    LayoutUIControl_ct(&this->sheetLabel);
    PushButton_ct(&this->launchButton);
    CtrlMenuBackButton_ct(&this->backButton);
    this->onBack.vt = &onBack_vt;
    this->onOption.vt = &onOption_vt;
    this->onSheetSelectRight.vt = &onSheetSelectRight_vt;
    this->onSheetSelectLeft.vt = &onSheetSelectLeft_vt;
    this->onLaunchButtonSelect.vt = &onLaunchButtonSelect_vt;
    this->onLaunchButtonFront.vt = &onLaunchButtonFront_vt;

    return this;
}

PATCH_B(TimeAttackGhostListPage_ct, my_TimeAttackGhostListPage_ct);

static void TimeAttackGhostListPage_dt(Page *base, s32 type) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    CtrlMenuBackButton_dt(&this->backButton, -1);
    PushButton_dt(&this->launchButton, -1);
    LayoutUIControl_dt(&this->sheetLabel, -1);
    SheetSelectControl_dt(&this->sheetSelect, -1);
    GhostSelectControl_dt(&this->ghostSelects[1], -1);
    GhostSelectControl_dt(&this->ghostSelects[0], -1);
    LayoutUIControl_dt(&this->switchLabel, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 TimeAttackGhostListPage_getReplacement(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    return this->replacement;
}

static void TimeAttackGhostListPage_onInit(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setPointerMode(&this->inputManager, 0x1);

    Page_initChildren(this, 8);
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->switchLabel, 0);
    Page_insertChild(this, 2, &this->ghostSelects[0], 0);
    Page_insertChild(this, 3, &this->ghostSelects[1], 0);
    Page_insertChild(this, 4, &this->sheetSelect, 0);
    Page_insertChild(this, 5, &this->sheetLabel, 0);
    Page_insertChild(this, 6, &this->launchButton, 0);
    Page_insertChild(this, 7, &this->backButton, 0);

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    const char *groups[] = {
        NULL,
        NULL,
    };
    LayoutUIControl_load(&this->switchLabel, "control", "ClassChange", "ClassChange", groups);
    GhostSelectControl_load(&this->ghostSelects[0]);
    GhostSelectControl_load(&this->ghostSelects[1]);
    SheetSelectControl_load(&this->sheetSelect, "button", "TimeAttackGhostListArrowRight", "ButtonArrowRight", "TimeAttackGhostListArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    LayoutUIControl_load(&this->sheetLabel, "control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    PushButton_load(&this->launchButton, "button", "TimeAttackGhostList", "Launch", 0x1, false, false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBackPopup", 0x1, false, true);

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false, false);
    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_OPTION, &this->onOption, false, false);
    SheetSelectControl_setRightHandler(&this->sheetSelect, &this->onSheetSelectRight);
    SheetSelectControl_setLeftHandler(&this->sheetSelect, &this->onSheetSelectLeft);
    PushButton_setSelectHandler(&this->launchButton, &this->onLaunchButtonSelect);
    PushButton_setFrontHandler(&this->launchButton, &this->onLaunchButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 0xd4f, NULL);
    u32 flags = RegisteredPadManager_getFlags(&s_sectionManager->registeredPadManager, 0);
    u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 0x902 : 0x901;
    ExtendedMessageInfo info = { .messageIds[0] = messageId };
    LayoutUIControl_setMessageAll(&this->switchLabel, 0xbc4, &info);
}

static void TimeAttackGhostListPage_onActivate(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    PushButton_selectDefault(&this->launchButton, 0);

    Section *currentSection = s_sectionManager->currentSection;
    this->ghostList = &((GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER])->list;

    u32 buttonsPerSheet = ARRAY_SIZE(this->ghostSelects[0].buttons);
    this->sheetCount = (this->ghostList->count + buttonsPerSheet - 1) / buttonsPerSheet;
    this->sheetIndex = 0;
    TimeAttackGhostListPage_refreshSheetLabel(this);

    this->chosenCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostIsChosen); i++) {
        this->ghostIsChosen[i] = false;
    }

    this->shownGhostSelect = &this->ghostSelects[0];
    this->hiddenGhostSelect = &this->ghostSelects[1];
    GhostSelectControl_hide(this->hiddenGhostSelect);
    GhostSelectControl_show(this->shownGhostSelect);

    this->lastSelected = -1;

    this->isReplay = false;
    this->switchLabel.isHidden = true;
    TimeAttackGhostListPage_refreshLaunchButton(this);

    this->replacement = -1; // TODO enum
}

static void TimeAttackGhostListPage_onRefocus(Page *base) {
    TimeAttackGhostListPage *this = (TimeAttackGhostListPage *)base;

    Section *currentSection = s_sectionManager->currentSection;
    if (!((RaceConfirmPage *)currentSection->pages[PAGE_ID_RACE_CONFIRM])->hasConfirmed) {
        return;
    }

    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    u32 playerCount = this->chosenCount;
    if (playerCount == 0 || !this->isReplay) {
        playerCount++;
    }
    for (u32 i = playerCount; i < ARRAY_SIZE(menuScenario->players); i++) {
        menuScenario->players[i].type = PLAYER_TYPE_NONE;
    }

    GlobalContext *cx = s_sectionManager->globalContext;
    cx->timeAttackGhostType = GHOST_TYPE_SAVED;
    cx->timeAttackCourseId = menuScenario->courseId;
    cx->timeAttackLicenseId = -1;
    if (playerCount != 0 && this->isReplay) {
        MiiGroup_copy(&cx->playerMiis, &cx->localPlayerMiis, 0, 11);
    }

    cx->timeAttackGhostCount = 0;
    for (u32 i = 0; i < ARRAY_SIZE(this->ghostIsChosen); i++) {
        if (this->ghostIsChosen[i]) {
            u32 index = this->ghostList->entries[i].index;
            cx->timeAttackGhostIndices[cx->timeAttackGhostCount++] = index;
        }
    }

    GhostManagerPage *ghostManagerPage = (GhostManagerPage *)currentSection->pages[PAGE_ID_GHOST_MANAGER];
    u32 sectionId; // TODO enum
    if (this->chosenCount == 0) {
        sectionId = 0x1f;
    } else if (this->isReplay) {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_REPLAY;
        sectionId = 0x34;
    } else {
        ghostManagerPage->nextRequest = GHOST_MANAGER_PAGE_REQUEST_SAVED_GHOST_RACE;
        sectionId = 0x1f;
    }
    this->vt->changeSection(this, sectionId, PAGE_ANIMATION_NEXT, 0.0f);
}

static const Page_vt s_TimeAttackGhostListPage_vt = {
    .dt = TimeAttackGhostListPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = TimeAttackGhostListPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = &Page_push,
    .onInit = TimeAttackGhostListPage_onInit,
    .vf_2c = &Page_vf_2c,
    .onActivate = TimeAttackGhostListPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .vf_48 = &Page_vf_48,
    .vf_4c = &Page_vf_4c,
    .vf_50 = &Page_vf_50,
    .onRefocus = TimeAttackGhostListPage_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .vf_60 = &Page_vf_60,
};

void TimeAttackGhostListPage_chooseGhost(TimeAttackGhostListPage *this, u32 buttonIndex) {
    u32 ghostIndex = this->sheetIndex * ARRAY_SIZE(this->ghostSelects[0].buttons) + buttonIndex;
    this->ghostIsChosen[ghostIndex] = this->shownGhostSelect->buttons[buttonIndex].chosen;

    if (this->shownGhostSelect->buttons[buttonIndex].chosen) {
        this->chosenCount++;
    } else {
        this->chosenCount--;
    }

    TimeAttackGhostListPage_refreshLaunchButton(this);
}
