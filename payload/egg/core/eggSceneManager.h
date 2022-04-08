#pragma once

#include <Common.h>
#include <egg/core/eggColorFader.h>

typedef struct EGG_SceneManager {
    char _00[0x0c - 0x00];
    struct EGGScene *curScene;
    char _10[0x24 - 0x10];
    EGG_ColorFader *fader;
    u32 _28;
} EGG_SceneManager;
static_assert(sizeof(EGG_SceneManager) == 0x2c);

extern void EGG_SceneManager_changeSiblingScene;

void SceneManager_destroyCurrentSceneNoIncoming(EGG_SceneManager *, bool destroyRoot);
void SceneManager_changeSiblingScene(EGG_SceneManager *, u32 sceneId);
