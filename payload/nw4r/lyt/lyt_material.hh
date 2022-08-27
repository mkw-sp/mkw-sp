#pragma once

extern "C" {
#include <revolution.h>
}

namespace nw4r::lyt {

class Material {
private:
    u8 _00[0x10 - 0x00];

public:
    GXColorS10 tevColors[3];

private:
    u8 _28[0x5c - 0x28];
};
static_assert(sizeof(Material) == 0x5c);

} // namespace nw4r::lyt
