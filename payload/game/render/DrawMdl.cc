#include "DrawMdl.hh"

namespace Render {

DrawMdl::~DrawMdl() {
    // This assumes there is a DrawMdl bound to each relevant patched model
    revertMirrorPatches();
}

void DrawMdl::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~DrawMdl();
    }
}

nw4r::g3d::ScnMdl **DrawMdl::scnMdl(u32 i) {
    return m_scnMdls[i];
}

void DrawMdl::revertMirrorPatches() {
    const u32 mask = 0xffff'ff00; // Account for two MIRR definitions adjacent (string
                                  // merging disabled for some reason)
    if ((m_resMdl.ref().ofsUserData & mask) != (reinterpret_cast<u32>(sTag_MIRR) & mask)) {
        return;
    }

    for (u32 i = 0; i < m_resMdl.GetResMatNumEntries(); ++i) {
        nw4r::g3d::ResGenMode gen = m_resMdl.GetResMat(i).GetResGenMode();
        switch (gen.GXGetCullMode()) {
        case GX_CULL_FRONT:
            gen.GXSetCullMode(GX_CULL_BACK);
            break;
        case GX_CULL_BACK:
            gen.GXSetCullMode(GX_CULL_FRONT);
            break;
        case GX_CULL_NONE:
        case GX_CULL_ALL:
            break;
        }
    }

    m_resMdl.ref().ofsUserData = 0;
}

void DrawMdl::applyMirrorPatches() {
    ApplyMirrorPatches(m_resMdl);
}

} // namespace Render
