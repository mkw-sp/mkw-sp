#include "FFAColors.hh"

namespace UI::FFAColors {

GXColor Get(u32 playerId) {
    GXColor colors[12] = {
            {163, 2, 0, 255},
            {255, 38, 0, 255},
            {255, 173, 2, 255},
            {48, 163, 2, 255},
            {0, 48, 7, 255},
            {0, 15, 117, 255},
            {0, 84, 183, 255},
            {232, 25, 51, 255},
            {56, 5, 104, 255},
            {30, 15, 2, 255},
            {167, 167, 167, 255},
            {0, 0, 0, 255},
    };
    return colors[playerId];
}

} // namespace UI::FFAColors
