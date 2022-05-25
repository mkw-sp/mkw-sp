#pragma once

#include "ControlGroup.h"
#include "MenuInputManager.h"
#include "TypeInfo.h"
#include "UIControl.h"

enum {
    PAGE_STATE_0,
    PAGE_STATE_1,
    PAGE_STATE_2,
    PAGE_STATE_3,
    PAGE_STATE_4,
    PAGE_STATE_5
};

enum {
    PAGE_ID_NONE = -0x1,
    
    PAGE_ID_GP_PAUSE_MENU = 0x17,
    PAGE_ID_VS_PAUSE_MENU = 0x18,
    PAGE_ID_TA_PAUSE_MENU = 0x19,
    PAGE_ID_BATTLE_PAUSE_MENU = 0x1a,
    PAGE_ID_MR_PAUSE_MENU = 0x1b,
    PAGE_ID_TA_GHOST_PAUSE_MENU = 0x1c,

    PAGE_ID_GHOST_REPLAY_PAUSE_MENU = 0x1f,

    PAGE_ID_AFTER_TA_MENU = 0x21,
    PAGE_ID_CONFIRM_QUIT = 0x2c,

    PAGE_ID_GP_REPLAY_PAUSE_MENU = 0x38,
    PAGE_ID_TA_REPLAY_PAUSE_MENU = 0x39,

    PAGE_ID_RACE_CONFIRM = 0x4b,
    PAGE_ID_MESSAGE_WINDOW_POPUP = 0x4d,
    PAGE_ID_CONFIRM = 0x52,
    PAGE_ID_TITLE = 0x57,
    PAGE_ID_TOP_MENU = 0x5a,
    PAGE_ID_LICENSE_SELECT = 0x65,
    PAGE_ID_LICENSE_SETTINGS = 0x67,
    PAGE_ID_SINGLE_TOP_MENU = 0x69,
    PAGE_ID_CHARACTER_SELECT = 0x6b,
    PAGE_ID_COURSE_SELECT = 0x6f,
    PAGE_ID_GHOST_MANAGER = 0xa7,

    // Replaced with MKW-SP license settings
    PAGE_ID_LICENSE_RECORDS = 0xce,
    // Disabled {
    PAGE_ID_LICENSE_RECORDS_FAVORITES = 0xcf,
    PAGE_ID_LICENSE_RECORDS_FRIENDS = 0xd0,
    PAGE_ID_LICENSE_RECORDS_WFC = 0xd1,
    PAGE_ID_LICENSE_RECORDS_OTHER = 0xd2,
    // }

    PAGE_ID_MAX = 0xd3,
};

enum {
    PAGE_ANIMATION_NEXT = 0x0,
    PAGE_ANIMATION_PREV = 0x1,
};

typedef struct PageBase {
    u32 id;
    u32 state;
    bool canProceed;
    u8 _0d[0x10 - 0xd];
    u8 _10[0x24 - 0x10];
    ControlGroup controlGroup;
    MenuInputManager *baseInputManager;
    u8 _3c[0x44 - 0x3c];
} PageBase;

typedef struct Page {
    const struct Page_vt *vt;
    BASE(PageBase);
} Page;
static_assert(sizeof(Page) == 0x44);

typedef struct Page_vt {
    u8 _00[0x08 - 0x00];
    void (*dt)(Page *this, s32 type);
    void *vf_0c;
    s32 (*getReplacement)(Page *this);
    void *vf_14;
    void *vf_18;
    void (*changeSection)(Page *this, u32 sectionId, s32 animation, f32 f1);
    void *vf_20;
    void (*push)(Page *this, u32 pageId, s32 animation);
    void (*onInit)(Page *this);
    void (*onDeinit)(Page *this);
    void (*onActivate)(Page *this);
    void *vf_34;
    void *vf_38;
    void *vf_3c;
    void *vf_40;
    void *vf_44;
    void (*beforeCalc)(Page *this);
    void (*afterCalc)(Page *this);
    void *vf_50;
    void (*onRefocus)(Page *this);
    void *vf_58;
    void *vf_5c;
    TypeInfo *(*getTypeInfo)(Page *this);
} Page_vt;
static_assert(sizeof(Page_vt) == 0x64);

Page *Page_ct(Page *this);

void Page_dt(Page *this, s32 type);

extern u8 Page_vf_0c;

s32 Page_getReplacement(Page *this);

extern u8 Page_vf_14;
extern u8 Page_vf_18;

void Page_changeSection(Page *this, u32 sectionId, s32 animation, f32 delay);

extern u8 Page_vf_20;

void Page_push(Page *this, u32 pageId, s32 animation);

void Page_onInit(Page *this);

void Page_onDeinit(Page *this);

void Page_onActivate(Page *this);

extern u8 Page_vf_34;
extern u8 Page_vf_38;
extern u8 Page_vf_3c;
extern u8 Page_vf_40;
extern u8 Page_vf_44;
extern void Page_beforeCalc(Page *this);
extern void Page_afterCalc(Page *this);
extern u8 Page_vf_50;

void Page_onRefocus(Page *this);

extern u8 Page_vf_58;
extern u8 Page_vf_5c;
TypeInfo *Page_getTypeInfo(Page *this);

void Page_initChildren(Page *this, u32 count);

void Page_insertChild(Page *this, u32 index, UIControl *child, u32 drawPass);

void Page_startReplace(Page *this, u32 animation, f32 delay);

// TODO r5
void Page_playSfx(Page *this, u32 sfxId, s32 r5);

void Page_update(Page *this);
void Page_animUpdate(Page *this);

void Page_init(Page *this, u32 pageId);

inline bool PageIsA(TypeInfo *typeInfo, Page *page) {
    assert(page != NULL);

    TypeInfo *pageTypeInfo = page->vt->getTypeInfo(page);

    return TypeInfo_isDerived(typeInfo, pageTypeInfo);
}

inline Page *PageAsA(TypeInfo *typeInfo, Page *page) {
    if (page == NULL)
        return NULL;

    // NB: In C++ this would just be a static_cast, which would add or subtract as needed
    // to cast Page to whatever type; in C that *doesn't* happen. So we just return
    // the Page pointer type and rely on the user to do the thisptr adjustment.
    return PageIsA(typeInfo, page) ? page : NULL;
}

// The port tool fails on these

// PAGE_ID_GHOST_MANAGER
// extern TypeInfo *GhostManagerPage_RTTI;
extern TypeInfo *GhostManagerPage_getTypeInfo();
#define GhostManagerPage_RTTI (GhostManagerPage_getTypeInfo())

// PauseMenu / PausePage probably
//
// RaceMenuPage + friends
// extern TypeInfo *PausePage_RTTI;
extern TypeInfo *RaceMenuPage_getTypeInfo();
#define RaceMenuPage_RTTI (RaceMenuPage_getTypeInfo())
