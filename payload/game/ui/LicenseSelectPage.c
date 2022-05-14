#include "LicenseSelectPage.h"

#include "Section.h"

#include "../system/SaveManager.h"

static const Page_vt s_LicenseSelectPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    s_saveManager->spCurrentLicense = -1;

    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onBack);
    page->replacement = PAGE_ID_TITLE;
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onServicePackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onServicePackButtonFront);
    f32 delay = PushButton_getDelay(button);
    Page_changeSection(page, SECTION_ID_CHANNEL, PAGE_ANIMATION_NEXT, delay);
}

static const PushButtonHandler_vt onServicePackButtonFront_vt = {
    .handle = onServicePackButtonFront,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    s_saveManager->spCurrentLicense = -1;

    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onBackButtonFront);
    page->replacement = PAGE_ID_TITLE;
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

static LicenseSelectPage *my_LicenseSelectPage_ct(LicenseSelectPage *this) {
    Page_ct(this);
    this->vt = &s_LicenseSelectPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    PushButton_ct(&this->servicePackButton);
    CtrlMenuBackButton_ct(&this->backButton);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        LicenseSelectButton_ct(&this->licenseButtons[i]);
    }
    this->onBack.vt = &onBack_vt;
    this->onServicePackButtonFront.vt = &onServicePackButtonFront_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    return this;
}
PATCH_B(LicenseSelectPage_ct, my_LicenseSelectPage_ct);

static void LicenseSelectPage_dt(Page *base, s32 type) {
    LicenseSelectPage *this = (LicenseSelectPage *)base;

    for (u32 i = ARRAY_SIZE(this->licenseButtons); i --> 0;) {
        LicenseSelectButton_dt(&this->licenseButtons[i], -1);
    }
    CtrlMenuBackButton_dt(&this->backButton, -1);
    PushButton_dt(&this->servicePackButton, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 LicenseSelectPage_getReplacement(Page *base) {
    LicenseSelectPage *this = (LicenseSelectPage *)base;

    return this->replacement;
}

static void LicenseSelectPage_onInit(Page *base) {
    LicenseSelectPage *this = (LicenseSelectPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_NEITHER);

    Page_initChildren(this, 3 + ARRAY_SIZE(this->licenseButtons));
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->servicePackButton, 0);
    Page_insertChild(this, 2, &this->backButton, 0);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        Page_insertChild(this, 3 + i, &this->licenseButtons[i], 0);
    }

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->servicePackButton, "button", "LicenseSelectS", "Option", 0x1, false,
            false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        LicenseSelectButton_load(&this->licenseButtons[i], i);
    }

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    PushButton_setFrontHandler(&this->servicePackButton, &this->onServicePackButtonFront, false);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    LayoutUIControl_setMessageAll(&this->servicePackButton, 10083, NULL);
    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 2104, NULL);
}

static void LicenseSelectPage_onActivate(Page *base) {
    LicenseSelectPage *this = (LicenseSelectPage *)base;

    u32 index = s_saveManager->spCurrentLicense < 0 ? 0 : s_saveManager->spCurrentLicense;
    PushButton_selectDefault(&this->licenseButtons[index], 0);

    this->replacement = PAGE_ID_NONE;
}

static const Page_vt s_LicenseSelectPage_vt = {
    .dt = LicenseSelectPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = LicenseSelectPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = LicenseSelectPage_onInit,
    .onDeinit = Page_onDeinit,
    .onActivate = LicenseSelectPage_onActivate,
    .vf_34 = &Page_vf_34,
    .vf_38 = &Page_vf_38,
    .vf_3c = &Page_vf_3c,
    .vf_40 = &Page_vf_40,
    .vf_44 = &Page_vf_44,
    .beforeCalc = &Page_beforeCalc,
    .afterCalc = &Page_afterCalc,
    .vf_50 = &Page_vf_50,
    .onRefocus = Page_onRefocus,
    .vf_58 = &Page_vf_58,
    .vf_5c = &Page_vf_5c,
    .getTypeInfo = Page_getTypeInfo,
};
