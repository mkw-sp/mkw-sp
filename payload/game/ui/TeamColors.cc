#include "TeamColors.hh"

namespace UI::TeamColors {

GXColor Get(u32 teamId) {
    GXColor colors[12] = {
        { 0, 85, 255, 255 },
        { 255, 0, 0, 255 },
        { 255, 255, 0, 255 },
        { 0, 255, 0, 255 },
        { 255, 0, 255, 255 },
        { 255, 255, 255, 255 },
        { 0, 114, 178, 255 },
        { 213, 94, 0, 255 },
        { 240, 228, 66, 255 },
        { 0, 158, 115, 255 },
        { 204, 121, 167, 255 },
        { 255, 255, 255, 255 },
    };
    return colors[teamId];
}

} // namespace UI::TeamColors
