#pragma once

#include "egg/core/eggXfb.hh"

namespace EGG {

class XfbManager {
public:
    Xfb *headXfb();
    void copyEFB(bool clear);

private:
    Xfb *m_headXfb;
    u8 _04[0x10 - 0x04];
};
static_assert(sizeof(XfbManager) == 0x10);

} // namespace EGG
