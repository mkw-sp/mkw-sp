#include <Common.h>

extern void EGG_SceneManager_changeSiblingScene;
extern void SceneManager_changeSiblingScene;

PATCH_B(SceneManager_changeSiblingScene + 0x4, EGG_SceneManager_changeSiblingScene);
