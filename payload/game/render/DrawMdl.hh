#pragma once

#include <nw4r/g3d/g3d_resmdl_ac.hh>
#include <nw4r/g3d/g3d_scnmdl.hh>

namespace Render {

class DrawMdl {
public:
    ~DrawMdl();

    virtual void vf00() = 0;
    virtual void vf04() = 0;

    REPLACE virtual void dt(s32 type);

    nw4r::g3d::ScnMdl **scnMdl(u32 i);

    void revertMirrorPatches();
    void applyMirrorPatches();

private:
    static const char *sTag_MIRR;
    static void ApplyMirrorPatches(nw4r::g3d::ResMdl &mdl);

public:
    u32 : 21;
    bool m_hasColorAnim : 1;
    u32 : 10;

private:
    u8 _08[0xc - 0x8];
    nw4r::g3d::ResMdl m_resMdl;
    u8 _10[0x14 - 0x10];
    nw4r::g3d::ScnMdl **m_scnMdls[2];
    u8 _1c[0x4c - 0x1c];
};
static_assert_32bit(sizeof(DrawMdl) == 0x4c);

} // namespace Render
