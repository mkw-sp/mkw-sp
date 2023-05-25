#pragma once

#include "nw4r/g3d/g3d_resmdl_ac.hh"

namespace nw4r::g3d {

class ScnMdl {
public:
    class CopiedMatAccess {
    public:
        CopiedMatAccess(ScnMdl *scnMdl, u32 idx);

        ResMatTevColor GetResMatTevColor(bool r4);

    private:
        u8 _00[0x34 - 0x00];
    };
    static_assert(sizeof(CopiedMatAccess) == 0x34);

private:
    // ...
};

} // namespace nw4r::g3d
