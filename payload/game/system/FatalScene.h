#pragma once

#include <egg/core/eggScene.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_pane.h>

enum {
    SCENE_ID_SPFATAL = 100,
};

typedef struct {
    EGGScene inherit;

    MEMAllocator allocator;
    lyt_DrawInfo drawInfo;

    lyt_MultiArcResourceAccessor resAccessor;
    lyt_ArcResourceLink arcLink;

    lyt_Layout layout;

    lyt_Pane *bodyPane;
} FatalScene;

FatalScene *FatalScene_CT(FatalScene *scene);
void FatalScene_DT(FatalScene *scene, s32 type);

// Set the main text
void FatalScene_SetBody(FatalScene *scene, const wchar_t *body);

// Take control of rendering
void FatalScene_MainLoop(FatalScene *scene);

// If its unsafe to unload the current scene, FatalScene can just repurpose its memory.
//
// Calling this + enter() is equivalent to SceneManager::create
void FatalScene_LeechCurrentScene(FatalScene *scene);
