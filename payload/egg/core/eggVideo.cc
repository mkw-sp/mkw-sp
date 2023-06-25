#include "eggVideo.hh"

extern "C" {
#include <sp/Host.h>
}

namespace EGG {

const GXRenderModeObj *Video::renderMode() const {
    return m_renderMode;
}

const GXRenderModeObj *Video::getStandardRenderModeObj(const GXRenderModeObj *const *renderModes) {
    auto *renderMode = REPLACED(getStandardRenderModeObj)(renderModes);
    if (!HostPlatform_IsDolphin(Host_GetPlatform()) || REGION == REGION_P) {
        return renderMode;
    }

    // Dolphin uses the "fallback region" for ELF executables. If that fallback region is PAL and
    // neither PAL60 nor progressive mode is enabled, NTSC discs will run at 50 Hz. We work around
    // around this by overriding the render mode.
    if (renderMode == renderModes[2]) {
        *reinterpret_cast<volatile u32 *>(0x800000cc) = 0;
        return renderModes[0];
    }
    if (renderMode == renderModes[7]) {
        *reinterpret_cast<volatile u32 *>(0x800000cc) = 0;
        return renderModes[5];
    }
    return renderMode;
}

} // namespace EGG
