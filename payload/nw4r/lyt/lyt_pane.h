#pragma once

#include "lyt_material.h"

typedef enum {
    kLytPaneFlag_IsVisible = 1 << 0,
} lyt_PaneFlags;

typedef struct lyt_Pane_Base {
    u8 _04[0x28 - 0x04];
    void *material;
    Vec3 trans;
    Vec3 rot;
    Vec2 scale;
    float width;
    float height;
    u8 _54[0xbb - 0x54];
    u8 mFlag;  // lyt_PaneFlags
    u8 _bc[0xd8 - 0xbc];
} lyt_Pane_Base;

typedef struct lyt_Pane {
    struct lyt_Pane_Vtable *vtable;
    BASE(lyt_Pane_Base);
} lyt_Pane;

static_assert(sizeof(lyt_Pane) == 0xd8);

typedef struct lyt_Pane_Vtable {
    char _00[0x3c - 0x00];
    lyt_Pane *(*FindPaneByName)(lyt_Pane *self, const char *name, bool recurse);
    char _68[0x68 - 0x40];
    lyt_Material *(*GetMaterial)(lyt_Pane *self);
    char _6c[0x74 - 0x6c];
} lyt_Pane_Vtable;
static_assert(offsetof(lyt_Pane_Vtable, FindPaneByName) == 0x3c);
static_assert(sizeof(lyt_Pane_Vtable) == 0x74);

#ifndef __cplusplus
static inline void lyt_setPaneVisible(lyt_Pane *pane, bool visible) {
    pane->mFlag &= ~kLytPaneFlag_IsVisible;
    pane->mFlag |= visible ? kLytPaneFlag_IsVisible : 0;
}
#endif

typedef struct lyt_TextBox {
    struct lyt_TextBox_Vtable* vtable;
    BASE(lyt_Pane_Base);
    wchar_t *buffer;
    // ...
} lyt_TextBox;

typedef struct lyt_TextBox_Vtable {
    BASE(lyt_Pane_Vtable);
    void *_74;
    void *FreeStringBuffer;
    void (*SetString)(lyt_TextBox* self, const wchar_t* str, u16 outPosition);
    void *SetString2;
} lyt_TextBox_Vtable;
static_assert(sizeof(lyt_TextBox_Vtable) == 0x84);
