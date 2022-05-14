#include <Common.S>

PATCH_B_START(EGG_SceneManager_createScene, 0x5c)
    mr r3, r30
    bl ResourceManager_onCreateScene

    lhz r0, 0x1c (r28) // Original instruction
    b EGG_SceneManager_createScene + 0x60
PATCH_B_END(EGG_SceneManager_createScene, 0x5c)
