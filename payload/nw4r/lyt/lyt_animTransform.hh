#pragma once

#include <Common.hh>

namespace nw4r::lyt {

class AnimTransform {
public:
    u16 getFrameSize() const;

private:
    u8 _00[0x14 - 0x00];
};
static_assert(sizeof(AnimTransform) == 0x14);

} // namespace nw4r::lyt
