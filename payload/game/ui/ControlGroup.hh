#pragma once

extern "C" {
#include <Common.h>
}

namespace UI {

class ControlGroup {
private:
    u8 _00[0x14 - 0x00];
};
static_assert(sizeof(ControlGroup) == 0x14);

} // namespace UI
