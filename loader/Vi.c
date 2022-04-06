#include "Vi.h"

#include <stdalign.h>

static u16 xfbWidth;
static u16 xfbHeight;
static alignas(0x4000) u32 xfb[304 * 542];

void Vi_init(void) {
    xfbWidth = 608;
    //xfbHeight = 542;
    xfbHeight = 456;

    for (u16 y = 0; y < xfbHeight; y++) {
        for (u16 x = 0; x < xfbWidth; x++) {
            Vi_writeGrayscaleToXfb(x, y, 16);
        }
    }
}

void Vi_writeGrayscaleToXfb(u16 x, u16 y, u8 intensity) {
    if (x > xfbWidth || y > xfbHeight) {
        return;
    }

    u32 *val = &xfb[y * (xfbWidth / 2) + x / 2];
    u8 y0 = *val >> 24;
    u8 y1 = *val >> 8;
    if (x & 1) {
        y1 = intensity;
    } else {
        y0 = intensity;
    }
    *val = y0 << 24 | 127 << 16 | y1 << 8 | 127;
}
