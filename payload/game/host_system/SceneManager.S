#include <Common.S>

PATCH_B_START(SceneManager_changeSiblingScene, 0x0)
    stw r5, 0x4 (r3)
    b EGG_SceneManager_changeSiblingScene
PATCH_B_END(SceneManager_changeSiblingScene, 0x0)
