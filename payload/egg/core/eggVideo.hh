#pragma once

extern "C" {
#include <revolution.h>
}

namespace EGG {

class Video {
public:
    const GXRenderModeObj *renderMode() const;

    static const GXRenderModeObj *REPLACED(getStandardRenderModeObj)(
            const GXRenderModeObj *const *renderModes);
    REPLACE static const GXRenderModeObj *getStandardRenderModeObj(
            const GXRenderModeObj *const *renderModes);

private:
    const GXRenderModeObj *m_renderMode;
    u8 _4[0xc - 0x4];
};
static_assert(sizeof(Video) == 0xc);

} // namespace EGG
