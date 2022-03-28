#include "CourseSelectControl.h"

#include "CourseSelectPage.h"

#include <stdio.h>

enum {
    GROUP_ID_SLIDE = 0x0,
};

// Slide
enum {
    ANIM_ID_HIDE = 0x0,
    ANIM_ID_SLIDE_IN_FROM_LEFT = 0x1,
    ANIM_ID_SLIDE_IN_FROM_RIGHT = 0x2,
    ANIM_ID_SHOW = 0x3,
    ANIM_ID_SLIDE_OUT_TO_LEFT = 0x4,
    ANIM_ID_SLIDE_OUT_TO_RIGHT = 0x5,
};

static const LayoutUIControl_vt s_CourseSelectControl_vt;

static void onButtonSelect(PushButtonHandler *this, PushButton *button, u32 UNUSED(localPlayerId)) {
    CourseSelectControl *control = CONTAINER_OF(this, CourseSelectControl, onButtonSelect);
    CourseSelectPage *page = (CourseSelectPage *)button->group->page;
    page->lastSelected = button->index;
    u32 courseId = page->sheetIndex * ARRAY_SIZE(control->buttons) + button->index;
    CourseSelectPage_selectCourse(page, courseId);
}

static const PushButtonHandler_vt onButtonSelect_vt = {
    .handle = onButtonSelect,
};

CourseSelectControl *CourseSelectControl_ct(CourseSelectControl *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_CourseSelectControl_vt;

    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        PushButton_ct(&this->buttons[i]);
    }
    this->onButtonSelect.vt = &onButtonSelect_vt;

    return this;
}

void CourseSelectControl_dt(UIControl *base, s32 type) {
    CourseSelectControl *this = (CourseSelectControl *)base;

    for (u32 i = ARRAY_SIZE(this->buttons); i --> 0;) {
        PushButton_dt(&this->buttons[i], -1);
    }

    LayoutUIControl_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static u32 CourseSelectControl_getSlideAnimation(CourseSelectControl *this) {
    return UIAnimator_getAnimation(&this->animator, GROUP_ID_SLIDE);
}

static void CourseSelectControl_setSlideAnimation(CourseSelectControl *this, u32 animationId) {
    UIAnimator_setAnimation(&this->animator, GROUP_ID_SLIDE, animationId, 0.0f);
}

static bool CourseSelectControl_hasSlidIn(CourseSelectControl *this) {
    switch (CourseSelectControl_getSlideAnimation(this)) {
    case ANIM_ID_SLIDE_IN_FROM_LEFT:
    case ANIM_ID_SLIDE_IN_FROM_RIGHT:
        return !UIAnimator_isActive(&this->animator, GROUP_ID_SLIDE);
    default:
        return false;
    }
}

void CourseSelectControl_calcSelf(UIControl *base) {
    CourseSelectControl *this = (CourseSelectControl *)base;

    if (CourseSelectControl_hasSlidIn(this)) {
        for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
            if (!this->buttons[i].isHidden) {
                PushButton_setPlayerFlags(&this->buttons[i], 0x1);
            }
        }

        CourseSelectPage *page = (CourseSelectPage *)this->group->page;
        if (this->buttons[page->lastSelected].isHidden) {
            PushButton_select(&this->buttons[0], 0);
        } else {
            PushButton_select(&this->buttons[page->lastSelected], 0);
        }

        CourseSelectControl_setSlideAnimation(this, ANIM_ID_SHOW);
    }

    this->isHidden = CourseSelectControl_getSlideAnimation(this) == ANIM_ID_HIDE;
}

static const LayoutUIControl_vt s_CourseSelectControl_vt = {
    .dt = CourseSelectControl_dt,
    .init = LayoutUIControl_init,
    .calc = LayoutUIControl_calc,
    .draw = LayoutUIControl_draw,
    .initSelf = UIControl_initSelf,
    .calcSelf = CourseSelectControl_calcSelf,
    .vf_20 = &UIControl_vf_20,
    .vf_24 = &UIControl_vf_24,
    .vf_28 = &LayoutUIControl_vf_28,
    .vf_2c = &LayoutUIControl_vf_2c,
    .vf_30 = &LayoutUIControl_vf_30,
    .vf_34 = &UIControl_vf_34,
    .vf_38 = &LayoutUIControl_vf_38,
};

void CourseSelectControl_load(CourseSelectControl *this) {
    const char *groups[] = {
        "Slide",
        "Hide",
        "SlideInFromLeft",
        "SlideInFromRight",
        "Show",
        "SlideOutToLeft",
        "SlideOutToRight",
        NULL,
        NULL,
    };
    LayoutUIControl_load(this, "control", "CourseSelectBase", "Base", groups);
    UIAnimator_setAnimation(&this->animator, GROUP_ID_SLIDE, ANIM_ID_HIDE, 0.0f);

    UIControl_initChildren(this, ARRAY_SIZE(this->buttons));
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        UIControl_insertChild(this, i, &this->buttons[i]);
        char variant[0x20];
        snprintf(variant, sizeof(variant), "Option%lu", i);
        PushButton_load(&this->buttons[i], "button", "CourseSelectOption", variant, 0x1, false,
                false);
        LayoutUIControl_setParentPane(&this->buttons[i], variant);
        this->buttons[i].index = i;
        PushButton_setSelectHandler(&this->buttons[i], &this->onButtonSelect);
    }
}

bool CourseSelectControl_isHidden(CourseSelectControl *this) {
    return CourseSelectControl_getSlideAnimation(this) == ANIM_ID_HIDE;
}

bool CourseSelectControl_isShown(CourseSelectControl *this) {
    return CourseSelectControl_getSlideAnimation(this) == ANIM_ID_SHOW;
}

static void CourseSelectControl_onHide(CourseSelectControl *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        PushButton_setPlayerFlags(&this->buttons[i], 0x0);
    }
}

static void CourseSelectControl_onShow(CourseSelectControl *this) {
    CourseSelectPage *page = (CourseSelectPage *)this->group->page;
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        u32 courseId = page->sheetIndex * ARRAY_SIZE(this->buttons) + i;
        if (courseId < 32) {
            this->buttons[i].isHidden = false;
            LayoutUIControl_setMessageAll(&this->buttons[i], 9300 + courseId, NULL);
        } else {
            this->buttons[i].isHidden = true;
        }
    }
}

void CourseSelectControl_hide(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_HIDE);
    CourseSelectControl_onHide(this);
}

void CourseSelectControl_slideInFromLeft(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_IN_FROM_LEFT);
    CourseSelectControl_onShow(this);
}

void CourseSelectControl_slideInFromRight(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_IN_FROM_RIGHT);
    CourseSelectControl_onShow(this);
}

void CourseSelectControl_show(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_SHOW);
    CourseSelectControl_onShow(this);
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        u32 flags = !this->buttons[i].isHidden ? 0x1 : 0x0;
        PushButton_setPlayerFlags(&this->buttons[i], flags);
    }
}

void CourseSelectControl_slideOutToLeft(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_OUT_TO_LEFT);
    CourseSelectControl_onHide(this);
}

void CourseSelectControl_slideOutToRight(CourseSelectControl *this) {
    CourseSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_OUT_TO_RIGHT);
    CourseSelectControl_onHide(this);
}
