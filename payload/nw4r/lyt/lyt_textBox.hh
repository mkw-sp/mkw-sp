#pragma once

#include "nw4r/lyt/lyt_pane.hh"

namespace nw4r::lyt {

class TextBox : public Pane {
private:
    u8 _0d8[0x104 - 0x0d8];
};
static_assert(sizeof(TextBox) == 0x104);

} // namespace nw4r::lyt
