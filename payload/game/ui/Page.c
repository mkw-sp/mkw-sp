#include "Page.h"

static bool s_PageTransitionsEnabled = true;

bool Page_getTransitionsEnabled(void) {
    return s_PageTransitionsEnabled;
}
void Page_setTransitionsEnabled(bool enable) {
    s_PageTransitionsEnabled = enable;
}

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
        if (!s_PageTransitionsEnabled)
            page->canProceed = true;
        break;
    }
}

PATCH_B(Page_update, my_Page_update);