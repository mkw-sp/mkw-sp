#include "CourseSelectPage.h"

#include "game/host_system/SystemManager.h"
#include "game/ui/SectionManager.h"
#include "sp/CourseManager.h"
#include "sp/Storage.h"

#include <revolution.h>
#include <wchar.h>

enum {
    MAX_TPL_SIZE = 554560,
};

static const Page_vt s_CourseSelectPage_vt;

static bool canGoBack(void) {
    switch (s_sectionManager->currentSection->id) {
    case SECTION_ID_SINGLE_VS_NEXT_COURSE:
    case SECTION_ID_SINGLE_BATTLE_NEXT_COURSE:
        return false;
    default:
        return true;
    }
}

static void onBack(InputHandler *this, u32 UNUSED(localPlayerId)) {
    if (!canGoBack()) {
        return;
    }

    CourseSelectPage *page = CONTAINER_OF(this, CourseSelectPage, onBack);
    Page_startReplace(page, PAGE_ANIMATION_PREV, 0.0f);
}

static const InputHandler_vt onBack_vt = {
    .handle = onBack,
};

static bool CourseSelectPage_canSwapCourseSelects(CourseSelectPage *this) {
    if (!CourseSelectControl_isShown(this->shownCourseSelect)) {
        return false;
    }

    if (!CourseSelectControl_isHidden(this->hiddenCourseSelect)) {
        return false;
    }

    return true;
}

static void CourseSelectPage_refreshSheetLabel(CourseSelectPage *this) {
    this->sheetLabel.isHidden = this->sheetCount == 0;
    MessageInfo info = {
        .intVals[0] = this->sheetIndex + 1,
        .intVals[1] = this->sheetCount,
    };
    LayoutUIControl_setMessageAll(&this->sheetLabel, 2009, &info);
}

static void CourseSelectPage_swapCourseSelects(CourseSelectPage *this) {
    CourseSelectControl *tmp = this->shownCourseSelect;
    this->shownCourseSelect = this->hiddenCourseSelect;
    this->hiddenCourseSelect = tmp;
}

static void onSheetSelectRight(SheetSelectControlHandler *this, SheetSelectControl *UNUSED(control),
        u32 UNUSED(localPlayerId)) {
    CourseSelectPage *page = CONTAINER_OF(this, CourseSelectPage, onSheetSelectRight);
    if (!CourseSelectPage_canSwapCourseSelects(page)) {
        return;
    }

    if (page->sheetIndex == page->sheetCount - 1) {
        page->sheetIndex = 0;
    } else {
        page->sheetIndex++;
    }
    CourseSelectPage_refreshSheetLabel(page);

    CourseSelectControl_slideOutToLeft(page->shownCourseSelect);
    CourseSelectControl_slideInFromRight(page->hiddenCourseSelect);

    CourseSelectPage_swapCourseSelects(page);
}

static const SheetSelectControlHandler_vt onSheetSelectRight_vt = {
    .handle = onSheetSelectRight,
};

static void onSheetSelectLeft(SheetSelectControlHandler *this, SheetSelectControl *UNUSED(control),
        u32 UNUSED(localPlayerId)) {
    CourseSelectPage *page = CONTAINER_OF(this, CourseSelectPage, onSheetSelectLeft);
    if (!CourseSelectPage_canSwapCourseSelects(page)) {
        return;
    }

    if (page->sheetIndex == 0) {
        page->sheetIndex = page->sheetCount - 1;
    } else {
        page->sheetIndex--;
    }
    CourseSelectPage_refreshSheetLabel(page);

    CourseSelectControl_slideOutToRight(page->shownCourseSelect);
    CourseSelectControl_slideInFromLeft(page->hiddenCourseSelect);

    CourseSelectPage_swapCourseSelects(page);
}

static const SheetSelectControlHandler_vt onSheetSelectLeft_vt = {
    .handle = onSheetSelectLeft,
};

static void onBackButtonFront(PushButtonHandler *this, PushButton *button,
        u32 UNUSED(localPlayerId)) {
    if (!canGoBack()) {
        return;
    }

    CourseSelectPage *page = CONTAINER_OF(this, CourseSelectPage, onBackButtonFront);
    f32 delay = PushButton_getDelay(button);
    Page_startReplace(page, PAGE_ANIMATION_PREV, delay);
}

static const PushButtonHandler_vt onBackButtonFront_vt = {
    .handle = onBackButtonFront,
};

extern void CupSelectPage_ct;

static CourseSelectPage *my_CourseSelectPage_ct(CourseSelectPage *this) {
    Page_ct(this);
    this->vt = &s_CourseSelectPage_vt;

    MultiControlInputManager_ct(&this->inputManager);
    LayoutUIControl_ct(&this->background);
    CtrlMenuPageTitleText_ct(&this->pageTitleText);
    CourseSelectControl_ct(&this->courseSelects[0]);
    CourseSelectControl_ct(&this->courseSelects[1]);
    SheetSelectControl_ct(&this->sheetSelect);
    LayoutUIControl_ct(&this->sheetLabel);
    CtrlMenuBackButton_ct(&this->backButton);
    this->onBack.vt = &onBack_vt;
    this->onSheetSelectRight.vt = &onSheetSelectRight_vt;
    this->onSheetSelectLeft.vt = &onSheetSelectLeft_vt;
    this->onBackButtonFront.vt = &onBackButtonFront_vt;
    this->tpl = spAlloc(MAX_TPL_SIZE, 0x20, NULL);

    return this;
}
PATCH_B(CupSelectPage_ct, my_CourseSelectPage_ct);

static void CourseSelectPage_dt(Page *base, s32 type) {
    CourseSelectPage *this = (CourseSelectPage *)base;

    spFree(this->tpl);
    CtrlMenuBackButton_dt(&this->backButton, -1);
    LayoutUIControl_dt(&this->sheetLabel, -1);
    SheetSelectControl_dt(&this->sheetSelect, -1);
    CourseSelectControl_dt(&this->courseSelects[1], -1);
    CourseSelectControl_dt(&this->courseSelects[0], -1);
    CtrlMenuPageTitleText_dt(&this->pageTitleText, -1);
    LayoutUIControl_dt(&this->background, -1);
    MultiControlInputManager_dt(&this->inputManager, -1);

    Page_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

static s32 CourseSelectPage_getReplacement(Page *UNUSED(base)) {
    return PAGE_ID_MACHINE_SELECT;
}

static void CourseSelectPage_onInit(Page *base) {
    CourseSelectPage *this = (CourseSelectPage *)base;

    MultiControlInputManager_init(&this->inputManager, 0x1, false);
    this->baseInputManager = &this->inputManager;
    MultiControlInputManager_setWrappingMode(&this->inputManager, WRAPPING_MODE_Y);

    Page_initChildren(this, 7);
    Page_insertChild(this, 0, &this->background, 0);
    Page_insertChild(this, 1, &this->pageTitleText, 0);
    Page_insertChild(this, 2, &this->courseSelects[0], 0);
    Page_insertChild(this, 3, &this->courseSelects[1], 0);
    Page_insertChild(this, 4, &this->sheetSelect, 0);
    Page_insertChild(this, 5, &this->sheetLabel, 0);
    Page_insertChild(this, 6, &this->backButton, 0);

    LayoutUIControl_load(&this->background, "bg", "CourseSelectBackground",
            "CourseSelectBackground", NULL);
    CtrlMenuPageTitleText_load(&this->pageTitleText, false);
    CourseSelectControl_load(&this->courseSelects[0]);
    CourseSelectControl_load(&this->courseSelects[1]);
    SheetSelectControl_load(&this->sheetSelect, "button", "CourseSelectArrowRight",
            "ButtonArrowRight", "CourseSelectArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    LayoutUIControl_load(&this->sheetLabel, "control", "CourseSelectPageNum", "CourseSelectPageNum",
            NULL);
    PushButton_load(&this->backButton, "button", "Back", "ButtonBack", 0x1, false, true);

    MultiControlInputManager_setHandler(&this->inputManager, INPUT_ID_BACK, &this->onBack, false,
            false);
    SheetSelectControl_setRightHandler(&this->sheetSelect, &this->onSheetSelectRight);
    SheetSelectControl_setLeftHandler(&this->sheetSelect, &this->onSheetSelectLeft);
    PushButton_setFrontHandler(&this->backButton, &this->onBackButtonFront, false);

    CtrlMenuPageTitleText_setMessage(&this->pageTitleText, 3405, NULL);
}

static void CourseSelectPage_onActivate(Page *base) {
    CourseSelectPage *this = (CourseSelectPage *)base;

    u32 buttonsPerSheet = ARRAY_SIZE(this->courseSelects[0].buttons);
    this->sheetCount = (CourseManager_count() + buttonsPerSheet - 1) / buttonsPerSheet;
    this->sheetIndex = 0;
    CourseSelectPage_refreshSheetLabel(this);

    this->shownCourseSelect = &this->courseSelects[0];
    this->hiddenCourseSelect = &this->courseSelects[1];
    CourseSelectControl_hide(this->hiddenCourseSelect);
    CourseSelectControl_show(this->shownCourseSelect);

    PushButton_selectDefault(&this->courseSelects[0].buttons[0], 0);
    this->lastSelected = 0;
    CourseSelectPage_selectCourse(this, 0);

    this->sheetSelect.isHidden = this->sheetCount <= 1;
    SheetSelectControl_setPlayerFlags(&this->sheetSelect, this->sheetCount <= 1 ? 0x0 : 0x1);
}

static const Page_vt s_CourseSelectPage_vt = {
    .dt = CourseSelectPage_dt,
    .vf_0c = &Page_vf_0c,
    .getReplacement = CourseSelectPage_getReplacement,
    .vf_14 = &Page_vf_14,
    .vf_18 = &Page_vf_18,
    .changeSection = Page_changeSection,
    .vf_20 = &Page_vf_20,
    .push = Page_push,
    .onInit = CourseSelectPage_onInit,
    .onDeinit = Page_onDeinit,
    .onActivate = CourseSelectPage_onActivate,
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

void CourseSelectPage_selectCourse(CourseSelectPage *this, u32 index) {
    this->background.isHidden = true;

    const wchar_t *aspectName = s_systemManager->aspectRatio ? L"169" : L"43";
    u32 dbId = CourseManager_getDbId(index);
    wchar_t path[48];
    swprintf(path, ARRAY_SIZE(path), L"/mkw-sp/thumbnails/%ls/%4u.tpl", aspectName, dbId);
    NodeInfo info;
    Storage_stat(path, &info);
    if (info.type != NODE_TYPE_FILE || info.size > MAX_TPL_SIZE) {
        return;
    }
    if (!Storage_fastReadFile(info.id, this->tpl, info.size, NULL)) {
        return;
    }

    lyt_Pane *pane = Layout_findPaneByName(&this->background.mainLayout, "background");
    assert(pane);
    lyt_Material *material = pane->vtable->GetMaterial(pane);
    assert(material);
    lyt_TexMap *texMap = lyt_Material_GetTexMapAry(material);
    assert(texMap);
    lyt_TexMap_ReplaceImage(texMap, this->tpl, 0);

    this->background.isHidden = false;
}
