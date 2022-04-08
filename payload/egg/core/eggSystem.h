#pragma once

#include <Common.h>
#include <egg/core/eggScene.h>
#include <egg/core/eggSceneManager.h>
#include <revolution.h>

typedef struct {
    GXRenderModeObj *renderMode;
} EGGVideo;

typedef struct {
    u8 flag;
    u8 _[3];
    struct EGGDisplay_Vtable *vt;
} EGGDisplay;

typedef struct EGGDisplay_Vtable {
    u32 _[2];
    void (*beginFrame)(EGGDisplay *);
    void (*beginRender)(EGGDisplay *);
    void (*endRender)(EGGDisplay *);
    void (*endFrame)(EGGDisplay *);
} EGGDisplay_Vtable;

typedef struct {
    char _[0x44 - 0x00];
    EGGVideo *video;
    void *xfbMgr;
    EGGDisplay *display;
    void *perfView;
    EGG_SceneManager *scnMgr;
    // ...
} EGGTSystem;

extern EGGTSystem sRKSystem;
