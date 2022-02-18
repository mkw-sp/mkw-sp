#pragma once

#include <Common.h>
#include <egg/core/eggColorFader.h>
#include <egg/core/eggHeap.h>

struct EGGScene_Vtable;

typedef struct {
    char _00[0x0c - 0x00];
    struct EGGScene *curScene;
    char _10[0x24 - 0x10];
    EGG_ColorFader *fader;
    u32 _28;
} EGG_SceneManager;
static_assert(sizeof(EGG_SceneManager) == 0x2c);

typedef struct EGGScene {
    struct EGGScene_Vtable *vt;
    u8 _disposer[0x10 - 0x04];
    EGG_Heap *heapParent;
    EGG_Heap *heapMem1;
    EGG_Heap *heapMem2;
    EGG_Heap *heapDebug;

    struct EGGScene *parentScene;
    struct EGGScene *firstChildScene;

    u32 sceneID;
    EGG_SceneManager *sceneMgr;
} EGGScene;

typedef struct EGGScene_Vtable {
    u32 _00[2];
    void (*dt)(EGGScene *scene, int type);
    void (*calc)(EGGScene *scene);
    void (*draw)(EGGScene *scene);
    void (*enter)(EGGScene *scene);
    void (*exit)(EGGScene *scene);
    void (*reinit)(EGGScene *scene);
    void (*incoming_childDestroy)(EGGScene *scene);
    void (*outgoing_childCreate)(EGGScene *scene);
} EGGScene_Vtable;

EGGScene *EGG_Scene_CT(EGGScene *scene);
void EGG_Scene_DT(EGGScene *scene, int param);
