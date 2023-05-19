#pragma once

#include <Common.hh>

namespace EGG {

class Effect {
public:
    void setColor(u8 r, u8 g, u8 b, u8 a);

private:
    u8 _00[0x7c - 0x00];
};
static_assert(sizeof(Effect) == 0x7c);

} // namespace EGG
