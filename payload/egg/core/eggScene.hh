#pragma once

#include <Common.hh>

namespace EGG {

class Scene {
    u8 _00[0x30 - 0x00];
};
static_assert(sizeof(Scene) == 0x30);

} // namespace EGG
