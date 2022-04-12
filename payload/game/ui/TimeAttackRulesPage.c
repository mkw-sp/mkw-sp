#include "TimeAttackRulesPage.h"

#include "SectionManager.h"

#include "page/CharacterSelectPage.h"

#include "../system/SaveManager.h"

#include <stdio.h>

static const Page_vt s_TimeAttackRulesPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onBack);
    page->replacement = PAGE_ID_SINGLE_TOP_MENU;
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onRuleControlFront(RadioButtonControlHandler *this, RadioButtonControl *control,
        u32 UNUSED(localPlayerId), s32 selected) {
    SpSaveLicense *license = s_saveManager->spLicenses[s_saveManager->spCurrentLicense];
    switch (control->index) {
    case 0:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleClass] = selected;
        break;
    case 1:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostSorting] = selected;
        break;
    case 2:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostTagVisibility] = selected;
        break;
    case 3:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostTagContent] = selected;
        break;
    case 4:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleSolidGhosts] = selected;
        break;
    case 5:
        s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostSound] = selected;
        break;
    }

    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onRuleControlFront);
    if (control->index < ARRAY_SIZE(page->ruleControls) - 1) {
        RadioButtonControl_select(&page->ruleControls[control->index + 1], 0);
    } else {
        PushButton_select(&page->okButton, 0);
    }
}

static const RadioButtonControlHandler_vt onRuleControlFront_vt = {
    .handle = onRuleControlFront,
};

static void onRuleControlSelect(RadioButtonControlHandler *this, RadioButtonControl *control,
        u32 UNUSED(localPlayerId), s32 selected) {
    if (selected < 0) {
        return;
    }

    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onRuleControlSelect);
    u32 messageIds[][4] = {
        { 10079, 10081 },
        { 10024, 10025, 10026, 10027 },
        { 10032, 10033, 10034 },
        { 10039, 10040, 10054, 10041 },
        { 10043, 10044, 10045 },
        { 10067, 10068, 10069 },
    };
    u32 messageId = messageIds[control->index][selected];
    CtrlMenuInstructionText_setMessage(&page->instructionText, messageId, NULL);
}

static const RadioButtonControlHandler_vt onRuleControlSelect_vt = {
    .handle = onRuleControlSelect,
};

static void onOkButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onOkButtonFront);
    page->replacement = PAGE_ID_CHARACTER_SELECT;
    Section *currentSection = s_sectionManager->currentSection;
    CharacterSelectPage *characterSelectPage =
            (CharacterSelectPage *)currentSection->pages[PAGE_ID_CHARACTER_SELECT];
    characterSelectPage->prevId = page->id;
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
}

static const PushButtonHandler_vt onOkButtonFront_vt = {
    .handle = onOkButtonFront,
};

static void onOkButtonSelect(PushButtonHandler *this, PushButton *UNUSED(button),
        u32 UNUSED(localPlayerId)) {
    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onOkButtonSelect);
    CtrlMenuInstructionText_setMessage(&page->instructionText, 10046, NULL);
}

static const PushButtonHandler_vt onOkButtonSelect_vt = {
    .handle = onOkButtonSelect,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    TimeAttackRulesPage *page = CONTAINER_OF(this, TimeAttackRulesPage, onBackButtonFront);
    page->replacement = PAGE_ID_SINGLE_TOP_MENU;
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

extern void ClassSelectPage_ct;

static TimeAttackRulesPage *TimeAttackRulesPage_ct(TimeAttackRulesPage *this) {
    Page_ct(this);
    this->vt = &s_TimeAttackRulesPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    PushButton_ct(&this->okButton);
    CtrlMenuInstructionText_ct(&this->instructionText);
    CtrlMenuBackButton_ct(&this->backButton);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        RadioButtonControl_ct(&this->ruleControls[i]);
    }
    this->onBack.vt = &onBack_vt;
    this->onRuleControlFront.vt = &onRuleControlFront_vt;
    this->onRuleControlSelect.vt = &onRuleControlSelect_vt;
    this->onOkButtonFront.vt = &onOkButtonFront_vt;
    this->onOkButtonSelect.vt = &onOkButtonSelect_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    return this;
}
PATCH_B(ClassSelectPage_ct, TimeAttackRulesPage_ct);

static void TimeAttackRulesPage_dt(Page *base, s32 type) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    for (u32 i = ARRAY_SIZE(this->ruleControls); i --> 0;) {
        RadioButtonControl_dt(&this->ruleControls[i], -1);
    }
    CtrlMenuBackButton_dt(&this->backButton, -1);
    CtrlMenuInstructionText_dt(&this->instructionText, -1);
    PushButton_dt(&this->okButton, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 TimeAttackRulesPage_getReplacement(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    return this->replacement;
}

static void TimeAttackRulesPage_onInit(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 4 + ARRAY_SIZE(this->ruleControls));
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->okButton, 0);
    Page_insertChild(this, 2, &this->instructionText, 0);
    Page_insertChild(this, 3, &this->backButton, 0);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        Page_insertChild(this, 4 + i, &this->ruleControls[i], 0);
    }

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->okButton, "button", "VSSetting", "ButtonOK", 0x1, false, false);
    CtrlMenuInstructionText_load(&this->instructionText);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);
    const char *ruleNames[] = {
        "Class",
        "GhostSorting",
        "GhostTagVisibility",
        "GhostTagContent",
        "SolidGhosts",
        "GhostSound",
    };
    u32 buttonCounts[] = { 2, 4, 3, 4, 3, 3 };
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        const SpSaveLicense *license = s_saveManager->spLicenses[s_saveManager->spCurrentLicense];
        assert(license != NULL);
        
        u32 chosen;
        switch (i) {
        case 0:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleClass];
            break;
        case 1:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostSorting];
            break;
        case 2:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostTagVisibility];
            break;
        case 3:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostTagContent];
            break;
        case 4:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleSolidGhosts];
            break;
        case 5:
            chosen = s_saveManager->iniSettings.mValues[kSetting_TaRuleGhostSound];
            break;
        }
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Radio%s", ruleNames[i]);
        char buffers[4][0x20];
        const char *buttonVariants[4];
        for (u32 j = 0; j < buttonCounts[i]; j++) {
            snprintf(buffers[j], sizeof(*buffers), "Option%s%lu", ruleNames[i], j);
            buttonVariants[j] = buffers[j];
        }
        RadioButtonControl_load(&this->ruleControls[i], buttonCounts[i], chosen, "control",
                "TASettingRadioBase", variant, "TASettingRadioOption", buttonVariants, 0x1, false,
                false);
        this->ruleControls[i].index = i;
    }

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    for (u32 i = 0; i < ARRAY_SIZE(this->ruleControls); i++) {
        RadioButtonControl *control = &this->ruleControls[i];
        RadioButtonControl_setFrontHandler(control, &this->onRuleControlFront);
        RadioButtonControl_setSelectHandler(control, &this->onRuleControlSelect);
    }
    PushButton_setFrontHandler(&this->okButton, &this->onOkButtonFront, false);
    PushButton_setSelectHandler(&this->okButton, &this->onOkButtonSelect);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 3400, NULL);
}

static void TimeAttackRulesPage_onDeinit(Page *UNUSED(base)) {
    SaveManagerProxy_markLicensesDirty(s_sectionManager->saveManagerProxy);
}

static void TimeAttackRulesPage_onActivate(Page *base) {
    TimeAttackRulesPage *this = (TimeAttackRulesPage *)base;

    PushButton_selectDefault(&this->okButton, 0);
    CtrlMenuInstructionText_setMessage(&this->instructionText, 10046, NULL);

    this->replacement = PAGE_ID_NONE;
}

static const Page_vt s_TimeAttackRulesPage_vt = {
    .dt = TimeAttackRulesPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = TimeAttackRulesPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = &Page_push,
    .onInit = TimeAttackRulesPage_onInit,
    .onDeinit = TimeAttackRulesPage_onDeinit,
    .onActivate = TimeAttackRulesPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .beforeCalc = Page_beforeCalc,
    .afterCalc = Page_afterCalc,
    .vf_50 = &Page_vf_50,
    .onRefocus = Page_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .getTypeInfo = Page_getTypeInfo,
};
