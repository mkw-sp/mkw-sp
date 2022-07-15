#include "RaceScene.hh"

extern "C" {
#include <revolution.h>
}

namespace Scene {

void RaceScene::vf_58() {
    OSTime start = OSGetTime();
    for (u32 i = 0; i < 157; i++) {
        calc();
    }
    OSTime duration = OSGetTime() - start;
    SP_LOG("%llu", OSTicksToMilliseconds(duration));
}

} // namespace Scene
