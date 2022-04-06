#include "Vi.h"

#include "Cache.h"

#include <stdalign.h>

static volatile u16 *vtr = (u16 *)0xcc002000;
static volatile u16 *dcr = (u16 *)0xcc002002;
static volatile u32 *vto = (u32 *)0xcc00200c;
static volatile u32 *vte = (u32 *)0xcc002010;
static volatile u32 *tfbl = (u32 *)0xcc00201c;
static volatile u32 *bfbl = (u32 *)0xcc002024;
static volatile u16 *visel = (u16 *)0xcc00206e;

static u16 vtrSave;
static u32 vtoSave;
static u32 vteSave;
static u16 xfbWidth;
static u16 xfbHeight;
static alignas(0x200) u32 xfb[320 * 574];

void Vi_init(void) {
    vtrSave = *vtr;
    vtoSave = *vto;
    vteSave = *vte;

    bool isProgressive = *visel & 0x1;
    bool isNtsc = (*dcr >> 8 & 3) == 0;
    xfbWidth = 640;
    xfbHeight = isProgressive || isNtsc ? 480 : 574;
    for (u16 y = 0; y < xfbHeight; y++) {
        for (u16 x = 0; x < xfbWidth; x++) {
            Vi_writeGrayscaleToXfb(x, y, 16);
        }
    }
    Vi_flushXfb();
    *vtr = xfbHeight << (3 + isProgressive) | (*vtr & 0xf);
    if (isProgressive) {
        *vto = 0x6 << 16 | 0x30;
        *vte = 0x6 << 16 | 0x30;
    } else if (isNtsc) {
        *vto = 0xf << 16 | 0x24;
        *vte = 0xe << 16 | 0x25;
    } else {
        *vto = 0x11 << 16 | 0x33;
        *vte = 0x10 << 16 | 0x34;
    }
    *tfbl = (u32)xfb;
    *bfbl = (u32)xfb;
}

u16 Vi_getXfbWidth(void) {
    return xfbWidth;
}

u16 Vi_getXfbHeight(void) {
    return xfbHeight;
}

u8 Vi_readGrayscaleFromXfb(u16 x, u16 y) {
    if (x > xfbWidth || y > xfbHeight) {
        return 16;
    }

    u32 val = xfb[y * (xfbWidth / 2) + x / 2];
    if (x & 1) {
        return val >> 8;
    } else {
        return val >> 24;
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

void Vi_flushXfb(void) {
    DCStoreRange(xfb, sizeof(xfb));
}

void Vi_deinit(void) {
    *vtr = vtrSave;
    *vto = vtoSave;
    *vte = vteSave;
}
