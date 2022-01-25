#pragma once

#include "MenuInputManager.h"
#include "UIControl.h"

enum {
    PAGE_ID_RACE_CONFIRM = 0x4b,
    PAGE_ID_CONFIRM = 0x52,
    PAGE_ID_CHARACTER_SELECT = 0x6b,
    PAGE_ID_GHOST_MANAGER = 0xa7,
    PAGE_ID_MAX = 0xd3,
};

enum {
    PAGE_ANIMATION_NEXT = 0x0,
    PAGE_ANIMATION_PREV = 0x1,
};

typedef struct Page {
    const struct Page_vt *vt;
    u32 id;
    u32 state;
    u8 _0c[0x38 - 0x0c];
    MenuInputManager *baseInputManager;
    u8 _3c[0x44 - 0x3c];
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
    void *vf_48;
    void *vf_4c;
    void *vf_50;
    void (*onRefocus)(Page *this);
    void *vf_58;
    void *vf_5c;
    void *vf_60;
} Page_vt;
static_assert(sizeof(Page_vt) == 0x64);

Page *Page_ct(Page *this);

void Page_dt(Page *this, s32 type);

extern u8 Page_vf_0c;

s32 Page_getReplacement(Page *this);

extern u8 Page_vf_14;
extern u8 Page_vf_18;

// TODO f1
void Page_changeSection(Page *this, u32 sectionId, s32 animation, f32 f1);

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
extern u8 Page_vf_48;
extern u8 Page_vf_4c;
extern u8 Page_vf_50;

void Page_onRefocus(Page *this);

extern u8 Page_vf_58;
extern u8 Page_vf_5c;
extern u8 Page_vf_60;

void Page_initChildren(Page *this, u32 count);

void Page_insertChild(Page *this, u32 index, UIControl *child, u32 drawPass);

void Page_startReplace(Page *this, u32 animation, f32 delay);
