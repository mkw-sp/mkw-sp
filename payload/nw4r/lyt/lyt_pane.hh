#pragma once

#include "nw4r/lyt/lyt_material.hh"

namespace nw4r::lyt {

class Pane {
public:
    virtual ~Pane();
    virtual void dt(s32 type);
    virtual void vf_0c();
    virtual void vf_10();
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual void setVtxColor(u32 index, GXColor color);
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();
    virtual void vf_38();
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();
    virtual void vf_4c();
    virtual void vf_50();
    virtual void vf_54();
    virtual void vf_58();
    virtual void vf_5c();
    virtual void vf_60();
    virtual void vf_64();
    virtual Material *getMaterial();
    virtual Material *getMaterial(u32 index);
    virtual void vf_70();

    void setVisible(bool visible);

private:
    u8 _04[0x2c - 0x04];

public:
    Vec3 m_trans;
    Vec3 m_rot;
    Vec2<f32> m_scale;
    f32 m_width;
    f32 m_height;
    Mtx34 m_localMtx;
    Mtx34 m_globalMtx;

private:
    u8 _b4[0xb8 - 0xb4];

public:
    u8 m_alpha;

private:
    u8 _b9[0xbb - 0xb9];
    u8 _bb : 7;

public:
    bool m_visible : 1;

private:
    u8 _bc[0xd8 - 0xbc];
};
static_assert(sizeof(Pane) == 0xd8);

} // namespace nw4r::lyt
