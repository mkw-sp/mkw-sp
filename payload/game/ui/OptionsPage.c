#include "OptionsPage.h"

#include "Section.h"

static const Page_vt s_OptionsPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    OptionsPage *page = CONTAINER_OF(this, OptionsPage, onBack);
    Page_changeSection(page, SECTION_ID_TITLE_FROM_OPTIONS, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    OptionsPage *page = CONTAINER_OF(this, OptionsPage, onBackButtonFront);
    f32 delay = PushButton_getDelay(button);
    Page_changeSection(page, SECTION_ID_TITLE_FROM_OPTIONS, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

extern void OptionsPage_ct;

static OptionsPage *my_OptionsPage_ct(OptionsPage *this) {
    Page_ct(this);
    this->vt = &s_OptionsPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    PushButton_ct(&this->settingsButton);
    PushButton_ct(&this->tracksButton);
    PushButton_ct(&this->updatesButton);
    PushButton_ct(&this->aboutButton);
    CtrlMenuBackButton_ct(&this->backButton);
    this->onBack.vt = &onBack_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    return this;
}
PATCH_B(OptionsPage_ct, my_OptionsPage_ct);

static void OptionsPage_dt(Page *base, s32 type) {
    OptionsPage *this = (OptionsPage *)base;

    CtrlMenuBackButton_dt(&this->backButton, -1);
    PushButton_dt(&this->aboutButton, -1);
    PushButton_dt(&this->updatesButton, -1);
    PushButton_dt(&this->tracksButton, -1);
    PushButton_dt(&this->settingsButton, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static void OptionsPage_onInit(Page *base) {
    OptionsPage *this = (OptionsPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 6);
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->settingsButton, 0);
    Page_insertChild(this, 2, &this->tracksButton, 0);
    Page_insertChild(this, 3, &this->updatesButton, 0);
    Page_insertChild(this, 4, &this->aboutButton, 0);
    Page_insertChild(this, 5, &this->backButton, 0);

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->settingsButton, "button", "OptionTopButton", "Settings", 0x1, false,
            false);
    PushButton_load(&this->tracksButton, "button", "OptionTopButton", "Tracks", 0x1, false, false);
    PushButton_load(&this->updatesButton, "button", "OptionTopButton", "Updates", 0x1, false,
            false);
    PushButton_load(&this->aboutButton, "button", "OptionTopButton", "About", 0x1, false, false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
        false);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 10082, NULL);

    PushButton_selectDefault(&this->settingsButton, 0);
}

static const Page_vt s_OptionsPage_vt = {
    .dt = OptionsPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = Page_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = OptionsPage_onInit,
    .onDeinit = Page_onDeinit,
    .onActivate = Page_onActivate,
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
