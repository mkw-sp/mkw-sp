#include "Page.h"

#include "../system/SaveManager.h"
#include "./SectionManager.h"

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
        if (!SaveManager_getSetting(s_saveManager, kSetting_PageTransitions))
            page->canProceed = true;
        break;
    }
}

static PageId my_Page_getReplacement(){
    if (s_sectionManager->currentSection->id == PAGE_ID_CONFIRM_QUIT)
        return 0x25;
    return 0x26;
}

PATCH_B(Page_update, my_Page_update);
PATCH_B(Page_getReplacement, my_Page_getReplacement);
