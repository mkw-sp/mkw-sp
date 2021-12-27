#include <Common.S>

.global my_SceneManager_changeSiblingScene
my_SceneManager_changeSiblingScene:
    stw r5, 0x4 (r3)
    b EGG_SceneManager_changeSiblingScene
.size my_SceneManager_changeSiblingScene, . - my_SceneManager_changeSiblingScene

PATCH_B(SceneManager_changeSiblingScene, my_SceneManager_changeSiblingScene)
