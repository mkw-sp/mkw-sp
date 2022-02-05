#pragma once

#include <Common.h>

typedef enum {
    kLytPaneFlag_IsVisible = 1 << 0,
} lyt_PaneFlags;

typedef struct {
    void *vtable;
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
} lyt_Pane;

static_assert(sizeof(lyt_Pane) == 0xd8);

inline void lyt_setPaneVisible(lyt_Pane *pane, bool visible) {
    pane->mFlag &= ~kLytPaneFlag_IsVisible;
    pane->mFlag |= visible ? kLytPaneFlag_IsVisible : 0;
}