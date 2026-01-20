#pragma once

#include "egg/gfx/DrawPathBase.hpp"
#include "egg/gfx/eggScnRootEx.hpp"

namespace EGG {

class ScnRenderer : public ScnRootEx {
public:
    DrawPathBase *getDrawPathBase(u16 path) const {
        // assert(path < getNumDrawPath());
        return mppPathSet[path];
    }

private:
    // ScnRendererConfig VT at 0xB4
    // CustomScnProc from 0xB8 to 0xC4
    char _b4[0xc4 - 0xb4];
    DrawPathBase **mppPathSet;
    char _c8[0xd0 - 0xc8];
};
static_assert(sizeof(ScnRenderer) == 0xd0);

} // namespace EGG
