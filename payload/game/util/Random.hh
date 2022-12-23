#pragma once

#include <Common.hh>

namespace Util {

class Random {
public:
    u32 nextU32();

private:
    u8 _00[0x18 - 0x00];
};
static_assert(sizeof(Random) == 0x18);

} // namespace Util
