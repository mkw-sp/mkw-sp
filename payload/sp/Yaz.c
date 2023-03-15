#include "Yaz.h"

#include <stdint.h>

enum {
    YAZ0_MAGIC = 0x59617a30,
    YAZ1_MAGIC = 0x59617a31,
};

static void writeU16(u8 *data, u32 offset, u16 val) {
    u8 *base = data + offset;
    base[0x0] = val >> 8;
    base[0x1] = val;
}

static void writeU32(u8 *data, u32 offset, u32 val) {
    u8 *base = data + offset;
    base[0x0] = val >> 24;
    base[0x1] = val >> 16;
    base[0x2] = val >> 8;
    base[0x3] = val;
}

u32 Yaz_encode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize) {
    if (dstSize < 0x10) {
        return 0;
    }
    writeU32(dst, 0x0, YAZ1_MAGIC);
    writeU32(dst, 0x4, srcSize);
    writeU32(dst, 0x8, 0x0);
    writeU32(dst, 0xc, 0x0);

    u32 srcOffset = 0x0, dstOffset = 0x10;
    u32 groupHeaderOffset;
    for (u32 i = 0; srcOffset < srcSize && dstOffset < dstSize; i = (i + 1) % 8) {
        if (i == 0) {
            groupHeaderOffset = dstOffset;
            dst[dstOffset++] = 0;
            if (dstOffset == dstSize) {
                return 0;
            }
        }
        u32 firstRefOffset = srcOffset < 0x1000 ? 0x0 : srcOffset - 0x1000;
        u32 bestRefSize = 0x1, bestRefOffset;
        for (u32 refOffset = firstRefOffset; refOffset < srcOffset; refOffset++) {
            u32 refSize;
            u32 maxRefSize = 0x111;
            if (srcSize - srcOffset < maxRefSize) {
                maxRefSize = srcSize - srcOffset;
            }
            if (dstSize - dstOffset < maxRefSize) {
                maxRefSize = dstSize - dstOffset;
            }
            if (bestRefSize < maxRefSize) {
                if (src[srcOffset + bestRefSize] != src[refOffset + bestRefSize]) {
                    continue;
                }
                for (refSize = 0; refSize < maxRefSize; refSize++) {
                    if (src[srcOffset + refSize] != src[refOffset + refSize]) {
                        break;
                    }
                }
                if (refSize > bestRefSize) {
                    bestRefSize = refSize;
                    bestRefOffset = refOffset;
                    if (bestRefSize == 0x111) {
                        break;
                    }
                }
            } else {
                break;
            }
        }
        if (bestRefSize < 0x3) {
            dst[groupHeaderOffset] |= 1 << (7 - i);
            dst[dstOffset++] = src[srcOffset++];
        } else {
            if (bestRefSize < 0x12) {
                if (dstOffset + sizeof(u16) > dstSize) {
                    return 0;
                }
                u16 val = (bestRefSize - 0x2) << 12 | (srcOffset - bestRefOffset - 0x1);
                writeU16(dst, dstOffset, val);
                dstOffset += sizeof(u16);
            } else {
                if (dstOffset + sizeof(u16) > dstSize) {
                    return 0;
                }
                writeU16(dst, dstOffset, srcOffset - bestRefOffset - 0x1);
                dstOffset += sizeof(u16);
                if (dstOffset + sizeof(u8) > dstSize) {
                    return 0;
                }
                dst[dstOffset++] = bestRefSize - 0x12;
            }
            srcOffset += bestRefSize;
        }
    }

    return srcOffset == srcSize ? dstOffset : 0;
}
