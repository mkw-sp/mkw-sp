#include "VI.hh"

#include <common/DCache.hh>

namespace VI {

extern "C" volatile u16 vtr;
extern "C" volatile u16 dcr;
extern "C" volatile u32 vto;
extern "C" volatile u32 vte;
extern "C" volatile u32 tfbl;
extern "C" volatile u32 bfbl;
extern "C" volatile u16 hsw;
extern "C" volatile u16 hsr;
extern "C" volatile u16 visel;

static u16 xfbWidth;
static u16 xfbHeight;
u32 *const xfb = reinterpret_cast<u32 *>(0x81600000);

void Init() {
    bool isProgressive = visel & 1 || dcr & 4;
    bool isNtsc = (dcr >> 8 & 3) == 0;
    xfbWidth = 640;
    xfbHeight = isProgressive || isNtsc ? 480 : 574;
    for (u16 y = 0; y < xfbHeight; y++) {
        for (u16 x = 0; x < xfbWidth; x++) {
            WriteGrayscaleToXFB(x, y, 16);
        }
    }
    FlushXFB();
    vtr = xfbHeight << (3 + isProgressive) | (vtr & 0xf);
    if (isProgressive) {
        vto = 0x6 << 16 | 0x30;
        vte = 0x6 << 16 | 0x30;
    } else if (isNtsc) {
        vto = 0x3 << 16 | 0x18;
        vte = 0x2 << 16 | 0x19;
    } else {
        vto = 0x1 << 16 | 0x23;
        vte = 0x0 << 16 | 0x24;
    }
    hsw = 0x2828;
    hsr = 0x10f5;
    tfbl = 1 << 28 | reinterpret_cast<u32>(xfb) >> 5;
    bfbl = 1 << 28 | reinterpret_cast<u32>(xfb) >> 5;
}

u16 GetXFBWidth() {
    return xfbWidth;
}

u16 GetXFBHeight() {
    return xfbHeight;
}

u8 ReadGrayscaleFromXFB(u16 x, u16 y) {
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

void WriteGrayscaleToXFB(u16 x, u16 y, u8 intensity) {
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

void FlushXFB() {
    DCache::Store(xfb, 320 * 574 * sizeof(u32));
}

} // namespace VI
