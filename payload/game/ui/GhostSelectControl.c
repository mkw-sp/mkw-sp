#include "GhostSelectControl.h"

#include "TimeAttackGhostListPage.h"

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

static const LayoutUIControl_vt s_GhostSelectControl_vt;

GhostSelectControl *GhostSelectControl_ct(GhostSelectControl *this) {
    LayoutUIControl_ct(this);
    this->vt = &s_GhostSelectControl_vt;

    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        GhostSelectButton_ct(&this->buttons[i]);
    }

    return this;
}

void GhostSelectControl_dt(UIControl *base, s32 type) {
    GhostSelectControl *this = (GhostSelectControl *)base;

    for (u32 i = ARRAY_SIZE(this->buttons); i --> 0;) {
        GhostSelectButton_dt(&this->buttons[i], -1);
    }

    LayoutUIControl_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static u32 GhostSelectControl_getSlideAnimation(GhostSelectControl *this) {
    return UIAnimator_getAnimation(&this->animator, GROUP_ID_SLIDE);
}

static void GhostSelectControl_setSlideAnimation(GhostSelectControl *this, u32 animationId) {
    UIAnimator_setAnimation(&this->animator, GROUP_ID_SLIDE, animationId, 0.0f);
}

static bool GhostSelectControl_hasSlidIn(GhostSelectControl *this) {
    switch (GhostSelectControl_getSlideAnimation(this)) {
    case ANIM_ID_SLIDE_IN_FROM_LEFT:
    case ANIM_ID_SLIDE_IN_FROM_RIGHT:
        return !UIAnimator_isActive(&this->animator, GROUP_ID_SLIDE);
    default:
        return false;
    }
}

void GhostSelectControl_calcSelf(UIControl *base) {
    GhostSelectControl *this = (GhostSelectControl *)base;

    if (GhostSelectControl_hasSlidIn(this)) {
        for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
            if (!this->buttons[i].isHidden) {
                TabOptionButton_setPlayerFlags(&this->buttons[i], 0x1);
            }
        }

        TimeAttackGhostListPage *page = (TimeAttackGhostListPage *)this->group->page;
        if (page->lastSelected == -1) {
            PushButton_select(&page->launchButton, 0);
        } else if (this->buttons[page->lastSelected].isHidden) {
            TabOptionButton_select(&this->buttons[0], 0);
        } else {
            TabOptionButton_select(&this->buttons[page->lastSelected], 0);
        }

        GhostSelectControl_setSlideAnimation(this, ANIM_ID_SHOW);
    }

    this->isHidden = GhostSelectControl_getSlideAnimation(this) == ANIM_ID_HIDE;
}

static const LayoutUIControl_vt s_GhostSelectControl_vt = {
    .dt = GhostSelectControl_dt,
    .init = LayoutUIControl_init,
    .calc = LayoutUIControl_calc,
    .draw = LayoutUIControl_draw,
    .initSelf = UIControl_initSelf,
    .calcSelf = GhostSelectControl_calcSelf,
    .vf_20 = &UIControl_vf_20,
    .vf_24 = &UIControl_vf_24,
    .vf_28 = &LayoutUIControl_vf_28,
    .vf_2c = &LayoutUIControl_vf_2c,
    .vf_30 = &LayoutUIControl_vf_30,
    .vf_34 = &UIControl_vf_34,
    .vf_38 = &LayoutUIControl_vf_38,
};

void GhostSelectControl_load(GhostSelectControl *this) {
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
    LayoutUIControl_load(this, "control", "GhostSelectBase", "Base", groups);
    UIAnimator_setAnimation(&this->animator, GROUP_ID_SLIDE, ANIM_ID_HIDE, 0.0f);

    UIControl_initChildren(this, ARRAY_SIZE(this->buttons));
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        UIControl_insertChild(this, i, &this->buttons[i]);
        GhostSelectButton_load(&this->buttons[i], i);
    }
}

bool GhostSelectControl_isHidden(GhostSelectControl *this) {
    return GhostSelectControl_getSlideAnimation(this) == ANIM_ID_HIDE;
}

bool GhostSelectControl_isShown(GhostSelectControl *this) {
    return GhostSelectControl_getSlideAnimation(this) == ANIM_ID_SHOW;
}

static void GhostSelectControl_onHide(GhostSelectControl *this) {
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        TabOptionButton_setPlayerFlags(&this->buttons[i], 0x0);
    }
}

static void GhostSelectControl_onShow(GhostSelectControl *this) {
    TimeAttackGhostListPage *page = (TimeAttackGhostListPage *)this->group->page;
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        u32 ghostIndex = page->sheetIndex * ARRAY_SIZE(this->buttons) + i;
        if (ghostIndex < page->ghostList->count) {
            this->buttons[i].isHidden = false;
            GhostSelectButton_refresh(&this->buttons[i], ghostIndex);
        } else {
            this->buttons[i].isHidden = true;
        }
    }
}

void GhostSelectControl_hide(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_HIDE);
    GhostSelectControl_onHide(this);
}

void GhostSelectControl_slideInFromLeft(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_IN_FROM_LEFT);
    GhostSelectControl_onShow(this);
}

void GhostSelectControl_slideInFromRight(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_IN_FROM_RIGHT);
    GhostSelectControl_onShow(this);
}

void GhostSelectControl_show(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_SHOW);
    GhostSelectControl_onShow(this);
    for (u32 i = 0; i < ARRAY_SIZE(this->buttons); i++) {
        u32 flags = !this->buttons[i].isHidden ? 0x1 : 0x0;
        TabOptionButton_setPlayerFlags(&this->buttons[i], flags);
    }
}

void GhostSelectControl_slideOutToLeft(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_OUT_TO_LEFT);
    GhostSelectControl_onHide(this);
}

void GhostSelectControl_slideOutToRight(GhostSelectControl *this) {
    GhostSelectControl_setSlideAnimation(this, ANIM_ID_SLIDE_OUT_TO_RIGHT);
    GhostSelectControl_onHide(this);
}
