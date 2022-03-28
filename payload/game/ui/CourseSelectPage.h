#pragma once

#include "CourseSelectControl.h"
#include "Page.h"
#include "SheetSelectControl.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    LayoutUIControl background;
    CtrlMenuPageTitleText pageTitleText;
    CourseSelectControl courseSelects[2];
    SheetSelectControl sheetSelect;
    LayoutUIControl sheetLabel;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    SheetSelectControlHandler onSheetSelectRight;
    SheetSelectControlHandler onSheetSelectLeft;
    PushButtonHandler onBackButtonFront;
    void *tpl;
    CourseSelectControl *shownCourseSelect;
    CourseSelectControl *hiddenCourseSelect;
    u32 sheetCount;
    u32 sheetIndex;
    u32 lastSelected;
} CourseSelectPage;

void CourseSelectPage_selectCourse(CourseSelectPage *this, u32 courseId);
