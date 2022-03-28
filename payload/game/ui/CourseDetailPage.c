#include "CourseDetailPage.h"

#include "SectionManager.h"

static const Page_vt s_CourseDetailPage_vt;

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    CourseDetailPage *page = CONTAINER_OF(this, CourseDetailPage, onBack);
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    CourseDetailPage *page = CONTAINER_OF(this, CourseDetailPage, onBackButtonFront);
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

extern void CourseSelectPage_ct;

static CourseDetailPage *CourseDetailPage_ct(CourseDetailPage *this) {
    Page_ct(this);
    this->vt = &s_CourseDetailPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    CtrlMenuBackButton_ct(&this->backButton);
    this->onBack.vt = &onBack_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;

    return this;
}
PATCH_B(CourseSelectPage_ct, CourseDetailPage_ct);

static void CourseDetailPage_dt(Page *base, s32 type) {
    CourseDetailPage *this = (CourseDetailPage *)base;

    CtrlMenuBackButton_dt(&this->backButton, -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 CourseDetailPage_getReplacement(Page *UNUSED(base)) {
    return PAGE_ID_COURSE_SELECT;
}

static void CourseDetailPage_onInit(Page *base) {
    CourseDetailPage *this = (CourseDetailPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 2);
    Page_insertChild(this, 0, &this->pageTitleText, 0);
    Page_insertChild(this, 1, &this->backButton, 0);

    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
        false);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 3405, NULL);
}

static const Page_vt s_CourseDetailPage_vt = {
    .dt = CourseDetailPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = CourseDetailPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = CourseDetailPage_onInit,
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
