#include "TimeAttackSplitsPage.h"

#include "../SectionManager.h"

#include "../../system/SaveManager.h"

s32 TimeAttackSplitsPage_getReplacement(TimeAttackSplitsPage *this);

s32 my_TimeAttackSplitsPage_getReplacement(TimeAttackSplitsPage *UNUSED(this)) {
    return PAGE_ID_AFTER_TA_MENU;
}
PATCH_B(TimeAttackSplitsPage_getReplacement, my_TimeAttackSplitsPage_getReplacement);

void TimeAttackSplitsPage_afterCalc(TimeAttackSplitsPage *this);

static void my_TimeAttackSplitsPage_afterCalc(TimeAttackSplitsPage *this) {
    if (this->state != 4) {
        return;
    }

    if (this->isReady) {
        return;
    }

    if (SaveManagerProxy_hasPendingRequests(s_sectionManager->saveManagerProxy)) {
        return;
    }

    if (s_saveManager->saveGhostResult) {
        LayoutUIControl_setMessageAll(&this->ghostMessage, 0x45b, NULL);
    } else {
        LayoutUIControl_setMessageAll(&this->ghostMessage, 0x45c, NULL);
    }
    this->isReady = true;
}
PATCH_B(TimeAttackSplitsPage_afterCalc, my_TimeAttackSplitsPage_afterCalc);

void TimeAttackSplitsPage_onFront(TimeAttackSplitsPage *this);

static void my_TimeAttackSplitsPage_onFront(TimeAttackSplitsPage *this) {
    if (!this->isReady) {
        return;
    }

    Page_startReplace(this, -1, 0.0f);
}
PATCH_B(TimeAttackSplitsPage_onFront, my_TimeAttackSplitsPage_onFront);
