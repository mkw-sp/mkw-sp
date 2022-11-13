#pragma once

#include <Common.hh>

namespace nw4r::lyt {

struct DrawInfo {
    u8 _00[0x44 - 0x00];
    Vec2<f32> locationAdjustScale;
    u8 _4c[0x50 - 0x4c];
    u8 _50_0 : 2;
    bool locationAdjust : 1;
    u8 _50_4 : 5;
};
static_assert(sizeof(DrawInfo) == 0x54);

} // namespace nw4r::lyt
