#pragma once

#include <game/ui/Layout.h>
#include "ControlGroup.h"
#include "Font.h"
#include "MiiGroup.h"
#include "UIAnimator.h"

#define LOG_CONTROL_LOAD false

typedef struct {
    const struct UIControl_vt *vt;
    u8 _04[0x64 - 0x04];
    ControlGroup *group;
    u8 _68[0x80 - 0x68];
    bool isHidden;
    u8 _81[0x98 - 0x81];
} UIControl;
static_assert(sizeof(UIControl) == 0x98);

typedef struct UIControl_vt {
    u8 _00[0x08 - 0x00];
    void (*dt)(UIControl *this, s32 type);
    void (*init)(UIControl *this);
    void (*calc)(UIControl *this);
    void (*draw)(UIControl *this);
    void (*initSelf)(UIControl *this);
    void (*calcSelf)(UIControl *this);
    void *vf_20;
    void *vf_24;
    void *vf_28;
    void *vf_2c;
    void *vf_30;
    void *vf_34;
} UIControl_vt;
static_assert(sizeof(UIControl_vt) == 0x38);

void UIControl_initSelf(UIControl *this);

void UIControl_calcSelf(UIControl *this);

extern u8 UIControl_vf_20;

extern u8 UIControl_vf_24;

extern u8 UIControl_vf_34;

void UIControl_initChildren(UIControl *this, u32 count);

void UIControl_insertChild(UIControl *this, u32 index, UIControl *child);

// TODO r5
void UIControl_playSfx(UIControl *this, u32 sfxId, s32 r5);

typedef struct {
    BASE(UIControl);
    UIAnimator animator;
    MainLayout mainLayout;
    u8 _144[0x174 - 0x144];
} LayoutUIControl;
static_assert(sizeof(LayoutUIControl) == 0x174);

typedef struct {
    BASE(UIControl_vt);
    void *vf_38;
} LayoutUIControl_vt;
static_assert(sizeof(LayoutUIControl_vt) == 0x3c);

LayoutUIControl *LayoutUIControl_ct(LayoutUIControl *this);

void LayoutUIControl_dt(UIControl *base, s32 type);

void LayoutUIControl_init(UIControl *base);

void LayoutUIControl_calc(UIControl *base);

void LayoutUIControl_draw(UIControl *base);

extern u8 LayoutUIControl_vf_28;

extern u8 LayoutUIControl_vf_2c;

extern u8 LayoutUIControl_vf_30;

extern u8 LayoutUIControl_vf_38;

void LayoutUIControl_load(LayoutUIControl *this, const char *dir, const char *file,
        const char *variant, const char *const *groups);

void LayoutUIControl_setParentPane(LayoutUIControl *this, const char *pane);

void LayoutUIControl_setMessage(LayoutUIControl *this, const char *pane, u32 messageId,
        MessageInfo *info);

void LayoutUIControl_setMessageAll(LayoutUIControl *this, u32 messageId, MessageInfo *info);

void LayoutUIControl_setPicture(LayoutUIControl *this, const char *dstPane, const char *srcPane);

bool LayoutUIControl_hasPictureSourcePane(LayoutUIControl *this, const char *pane);

void LayoutUIControl_setMiiPicture(LayoutUIControl *this, const char *pane, MiiGroup *miiGroup,
        u32 index, u32 preset);

void LayoutUIControl_setPaneVisible(LayoutUIControl *this, const char *pane, bool visible);
