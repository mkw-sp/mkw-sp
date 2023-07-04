#pragma once

extern "C" {
#include <revolution.h>
}

namespace EGG {

class Display {
public:
    /* virtual */ void beginFrame();
    /* virtual */ void beginRender();
    /* virtual */ void endRender();
    REPLACE /* virtual */ void endFrame();
    /* virtual */ u32 getTickPerFrame();

protected:
    REPLACE void copyEFBtoXFB();

private:
    u8 : 7;
    bool m_clear : 1;
    u8 _04[0x14 - 0x04];
    GXColor m_clearColor;
    u32 m_clearZ;
    u8 _1c[0x28 - 0x1c];
};
static_assert(sizeof(Display) == 0x28);

} // namespace EGG
