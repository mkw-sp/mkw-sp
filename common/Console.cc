#include "Console.hh"

#include "Font.hh"
#include "VI.hh"

namespace Console {

static const u8 bg = 16;
static const u8 fg = 235;
static u8 cols;
static u8 rows;
static u8 col;
static u8 row;

void Init() {
    cols = VI::GetXFBWidth() / Font::GetGlyphWidth() - 1;
    rows = VI::GetXFBHeight() / Font::GetGlyphHeight() / (!VI::IsProgressive() + 1) - 1;
    col = 0;
    row = 0;
}

static void Print(char c) {
    if (col >= cols) {
        return;
    }

    if (c == '\n') {
        col = 0;
        row++;
        return;
    }

    if (c == '\r') {
        col = 0;
        return;
    }

    while (row >= rows) {
        u16 xfbWidth = VI::GetXFBWidth();
        u8 glyphHeight = Font::GetGlyphHeight();
        for (u8 row = 0; row < rows; row++) {
            u16 y0 = row * glyphHeight + glyphHeight / 2;
            for (u16 y = 0; y < glyphHeight; y++) {
                for (u16 x = 0; x < xfbWidth; x++) {
                    u8 intensity = VI::ReadGrayscaleFromXFB(x, y0 + glyphHeight + y);
                    VI::WriteGrayscaleToXFB(x, y0 + y, intensity);
                }
            }
        }
        row--;
    }

    u8 glyphWidth = Font::GetGlyphWidth();
    u8 glyphHeight = Font::GetGlyphHeight();
    const u8 *glyph = Font::GetGlyph(c);
    u16 y0 = row * glyphHeight + glyphHeight / 2;
    for (u16 y = 0; y < glyphHeight; y++) {
        u16 x0 = col * glyphWidth + glyphWidth / 2;
        for (u16 x = 0; x < glyphWidth; x++) {
            u8 intensity = glyph[(y * glyphWidth + x) / 8] & (1 << (8 - (x % 8))) ? fg : bg;
            VI::WriteGrayscaleToXFB(x0 + x, y0 + y, intensity);
        }
    }

    col++;
}

void Print(const char *s) {
    for (; *s; s++) {
        Print(*s);
    }
    VI::FlushXFB();
}

void Print(u32 val) {
    for (u32 i = 0; i < 8; i++) {
        u32 digit = val >> ((7 - i) * 4) & 0xf;
        if (digit < 0xa) {
            Print(static_cast<char>(digit + '0'));
        } else {
            Print(static_cast<char>(digit - 0xa + 'A'));
        }
    }
}

} // namespace Console

extern "C" void Console_Print(const char *s) {
    Console::Print(s);
}
