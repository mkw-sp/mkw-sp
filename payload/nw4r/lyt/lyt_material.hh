#pragma once

#include "nw4r/lyt/lyt_texMap.hh"

extern "C" {
#include <revolution.h>
}

#include <Common.hh>

namespace nw4r::lyt {

struct TexSRT {
    Vec2<f32> translate;
    f32 rotate;
    Vec2<f32> scale;
};
static_assert(sizeof(TexSRT) == 0x14);

class Material {
public:
    u8 getTextureNum() const;
    u8 getTexSRTNum() const;
    TexMap *getTexMapAry();
    TexSRT *getTexSRTAry();

private:
    u8 _00[0x10 - 0x00];

public:
    GXColorS10 tevColors[3];

private:
    u8 _28[0x3c - 0x28];
    u32 _3c;
    u8 _40[0x5c - 0x40];
};
static_assert(sizeof(Material) == 0x5c);

} // namespace nw4r::lyt
