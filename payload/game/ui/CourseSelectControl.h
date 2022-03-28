#pragma once

#include "Button.h"

typedef struct {
    LayoutUIControl;
    PushButton buttons[7];
    PushButtonHandler onButtonSelect;
} CourseSelectControl;

CourseSelectControl *CourseSelectControl_ct(CourseSelectControl *this);

void CourseSelectControl_dt(UIControl *base, s32 type);

void CourseSelectControl_load(CourseSelectControl *this);

bool CourseSelectControl_isHidden(CourseSelectControl *this);

bool CourseSelectControl_isShown(CourseSelectControl *this);

void CourseSelectControl_hide(CourseSelectControl *this);

void CourseSelectControl_slideInFromLeft(CourseSelectControl *this);

void CourseSelectControl_slideInFromRight(CourseSelectControl *this);

void CourseSelectControl_show(CourseSelectControl *this);

void CourseSelectControl_slideOutToLeft(CourseSelectControl *this);

void CourseSelectControl_slideOutToRight(CourseSelectControl *this);
