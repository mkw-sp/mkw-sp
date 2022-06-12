#include "Font.hh"

extern "C" {
#include "Font.h"
}

namespace Font {

u8 GetGlyphWidth() {
    return 8;
}

u8 GetGlyphHeight() {
    return 16;
}

const u8 *GetGlyph(char c) {
    u32 i = c;
    if (i < 128) {
        return font[i];
    }

    return font[' '];
}

} // namespace Font
