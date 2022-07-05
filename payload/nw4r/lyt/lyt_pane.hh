#pragma once

#include <Common.hh>

namespace nw4r::lyt {

class Pane {
public:
    void setVisible(bool visible);

private:
    u8 _00[0x2c - 0x00];

public:
    Vec3 m_trans;
    Vec3 m_rot;
    Vec2 m_scale;
    f32 m_width;
    f32 m_height;

private:
    u8 _54[0xbb - 0x54];
    u8 _bb : 7;

public:
    bool m_visible : 1;

private:
    u8 _bc[0xd8 - 0xbc];
};
static_assert(sizeof(Pane) == 0xd8);

} // namespace nw4r::lyt
