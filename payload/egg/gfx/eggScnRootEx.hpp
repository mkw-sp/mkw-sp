#pragma once

namespace EGG {

class ScnRootEx {
private:
    u8 _00[0xb4 - 0x00];
};
static_assert(sizeof(ScnRootEx) == 0xb4);

} // namespace EGG
