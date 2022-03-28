#pragma once

#include "Page.h"

#include "ctrl/CtrlMenuBackButton.h"
#include "ctrl/CtrlMenuPageTitleText.h"

typedef struct {
    Page;
    MultiControlInputManager inputManager;
    CtrlMenuPageTitleText pageTitleText;
    CtrlMenuBackButton backButton;
    InputHandler onBack;
    PushButtonHandler onBackButtonFront;
} CourseDetailPage;
