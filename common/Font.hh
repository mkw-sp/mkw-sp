#pragma once

#include <Common.hh>

namespace Font {

u8 GetGlyphWidth();

u8 GetGlyphHeight();

const u8 *GetGlyph(char c);

} // namespace Font
