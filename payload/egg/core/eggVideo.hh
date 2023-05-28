#pragma once

extern "C" {
#include <revolution.h>
}

namespace EGG {

class Video {
public:
    static const GXRenderModeObj *REPLACED(getStandardRenderModeObj)(
            const GXRenderModeObj *const *renderModes);
    REPLACE static const GXRenderModeObj *getStandardRenderModeObj(
            const GXRenderModeObj *const *renderModes);

private:
    u8 _0[0xc - 0x0];
};
static_assert(sizeof(Video) == 0xc);

} // namespace EGG
