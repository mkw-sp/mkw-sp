#pragma once

#include <nw4r/g3d/g3d_resmdl_ac.hh>

namespace Render {

class DrawMdl {
public:
    ~DrawMdl();

    virtual void vf00() = 0;
    virtual void vf04() = 0;

    REPLACE virtual void dt(s32 type);
    void revertMirrorPatches();
    void applyMirrorPatches();

private:
    static const char *sTag_MIRR;
    static void ApplyMirrorPatches(nw4r::g3d::ResMdl &mdl);

    u8 _08[0xc - 0x4];
    nw4r::g3d::ResMdl m_resMdl;
    u8 _10[0x4c - 0x10];
};

static_assert_32bit(sizeof(DrawMdl) == 0x4c);

} // namespace Render
