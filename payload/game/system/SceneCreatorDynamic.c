#include "FatalScene.h"
#include <revolution.h>

EGGScene *SceneCreatorDynamic_createOther(void * /* this */, u32 sceneId) {
    OSReport("SceneCreatorDynamic_createOther %u\n", sceneId);

    switch (sceneId) {
    case SCENE_ID_SPFATAL:;
        FatalScene *scn = new (sizeof(FatalScene));
        FatalScene_CT(scn);
        return (EGGScene *)scn;
    }

    return NULL;
}
