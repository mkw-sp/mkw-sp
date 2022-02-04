#include "LicenseSelectPage.h"

#include "SectionManager.h"

#include "../system/SaveManager.h"

static const Page_vt s_LicenseSelectPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onBack);
    page->replacement = PAGE_ID_TITLE;
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onAboutButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onAboutButtonFront);
    Section *currentSection = s_sectionManager->currentSection;
    ConfirmPage *confirmPage = (ConfirmPage *)currentSection->pages[PAGE_ID_CONFIRM];
    ConfirmPage_reset(confirmPage);
    ConfirmPage_setTitleMessage(confirmPage, 0x1b58, NULL);
    ExtendedMessageInfo info = {
        .strings[0] = L"MKW-SP v0.1",
    };
    ConfirmPage_setWindowMessage(confirmPage, 0x19ca, &info);
    confirmPage->onConfirm = &page->onAboutConfirm;
    confirmPage->onCancel = &page->onAboutConfirm;
    page->replacement = PAGE_ID_CONFIRM;
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_NEXT, delay);
}

static const PushButtonHandler_vt onAboutButtonFront_vt = {
    .handle = onAboutButtonFront,
};

static void onAboutConfirm(ConfirmPageHandler *this, ConfirmPage *confirmPage, f32 UNUSED(delay)) {
    LicenseSelectPage *page = CONTAINER_OF(this, LicenseSelectPage, onAboutConfirm);
    page->replacement = PAGE_ID_NONE;
    confirmPage->replacement = PAGE_ID_LICENSE_SELECT;
}

static const ConfirmPageHandler_vt onAboutConfirm_vt = {
    .handle = onAboutConfirm,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
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
    PushButton_ct(&this->aboutButton);
    CtrlMenuBackButton_ct(&this->backButton);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        LicenseSelectButton_ct(&this->licenseButtons[i]);
    }
    this->onBack.vt = &onBack_vt;
    this->onAboutButtonFront.vt = &onAboutButtonFront_vt;
    this->onAboutConfirm.vt = &onAboutConfirm_vt;
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
    PushButton_dt(&this->aboutButton, -1);
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
    MultiControlInputManager_setPointerMode(&this->inputManager, 0x1);

    Page_initChildren(this, 3 + ARRAY_SIZE(this->licenseButtons));
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->aboutButton, 0);
    Page_insertChild(this, 2, &this->backButton, 0);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        Page_insertChild(this, 3 + i, &this->licenseButtons[i], 0);
    }

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->aboutButton, "button", "LicenseSelectS", "Option", 0x1, false, false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);
    for (u32 i = 0; i < ARRAY_SIZE(this->licenseButtons); i++) {
        LicenseSelectButton_load(&this->licenseButtons[i], i);
    }

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    PushButton_setFrontHandler(&this->aboutButton, &this->onAboutButtonFront, false);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 0x838, NULL);
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
    .vf_60 = &Page_vf_60,
};
