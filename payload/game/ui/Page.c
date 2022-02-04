#include "Page.h"

#include "../system/SaveManager.h"

static void my_Page_update(Page *page) {
    switch (page->state) {
    case PAGE_STATE_3:
    case PAGE_STATE_4:
    case PAGE_STATE_5:
        // Handle animations
        (*page->vt->beforeCalc)(page);
        ControlGroup_calc(&page->controlGroup);
        (*page->vt->afterCalc)(page);
        Page_animUpdate(page);
        break;
    }

    // Override animation for instant transition
    switch (page->state) {
    case PAGE_STATE_3:
    case PAGE_STATE_5:
        if (!SaveManager_getSettingPageTransitions(s_saveManager))
            page->canProceed = true;
        break;
    }
}
PATCH_B(Page_update, my_Page_update);
