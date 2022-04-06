#include "Console.h"

#include "Font.h"
#include "Vi.h"

static const u8 bg = 16;
static const u8 fg = 235;
static u8 cols;
static u8 rows;
static u8 col;
static u8 row;

void Console_init(void) {
    cols = Vi_getXfbWidth() / Font_getGlyphWidth() - 1;
    rows = Vi_getXfbHeight() / Font_getGlyphHeight() - 1;
    col = 0;
    row = 0;
}

static void Console_printChar(char c) {
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
        u16 xfbWidth = Vi_getXfbWidth();
        u8 glyphHeight = Font_getGlyphHeight();
        for (u8 row = 0; row < rows; row++) {
            u16 y0 = row * glyphHeight + glyphHeight / 2;
            for (u16 y = 0; y < glyphHeight; y++) {
                for (u16 x = 0; x < xfbWidth; x++) {
                    u8 intensity = Vi_readGrayscaleFromXfb(x, y0 + glyphHeight + y);
                    Vi_writeGrayscaleToXfb(x, y0 + y, intensity);
                }
            }
        }
        row--;
    }

    u8 glyphWidth = Font_getGlyphWidth();
    u8 glyphHeight = Font_getGlyphHeight();
    const u8 *glyph = Font_getGlyph(c);
    u16 y0 = row * glyphHeight + glyphHeight / 2;
    for (u16 y = 0; y < glyphHeight; y++) {
        u16 x0 = col * glyphWidth + glyphWidth / 2;
        for (u16 x = 0; x < glyphWidth; x++) {
            u8 intensity = glyph[(y * glyphWidth + x) / 8] & (1 << (8 - (x % 8))) ? fg : bg;
            Vi_writeGrayscaleToXfb(x0 + x, y0 + y, intensity);
        }
    }

    col++;
}

void Console_printString(const char *s) {
    for (; *s; s++) {
        Console_printChar(*s);
    }
    Vi_flushXfb();
}
