#include "Yaz.h"

#include <stdint.h>

enum {
    YAZ0_MAGIC = 0x59617a30,
    YAZ1_MAGIC = 0x59617a31,
};

static u16 readU16(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 8 | base[0x1];
}

static u32 readU32(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 24 | base[0x1] << 16 | base[0x2] << 8 | base[0x3];
}

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

s32 Yaz_getDecodedSize(const u8 *src, u32 srcSize) {
    if (srcSize < 0x10) {
        return -1;
    }

    u32 magic = readU32(src, 0x0);
    if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
        return -1;
    }
    return readU32(src, 0x4);
}

u32 Yaz_decode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize) {
    if (srcSize < 0x10) {
        return 0;
    }

    u32 magic = readU32(src, 0x0);
    if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
        return 0;
    }
    if (readU32(src, 0x4) < dstSize) {
        dstSize = readU32(src, 0x4);
    }

    u32 srcOffset = 0x10, dstOffset = 0x0;
    u8 groupHeader;
    for (u8 i = 0; srcOffset < srcSize && dstOffset < dstSize; i = (i + 1) % 8) {
        if (i == 0) {
            groupHeader = src[srcOffset++];
            if (srcOffset == srcSize) {
                return dstOffset;
            }
        }
        if (groupHeader >> (7 - i) & 1) {
            dst[dstOffset++] = src[srcOffset++];
        } else {
            if (srcOffset + 2 > srcSize) {
                return dstOffset;
            }
            u16 val = readU16(src, srcOffset);
            srcOffset += 0x2;
            u32 refOffset = dstOffset - (val & 0xfff) - 0x1;
            if (refOffset >= dstOffset) {
                return dstOffset;
            }
            u16 refSize = (val >> 12) + 0x2;
            if (refSize == 0x2) {
                if (srcOffset + 1 > srcSize) {
                    return dstOffset;
                }
                refSize = src[srcOffset++] + 0x12;
            }
            if (dstOffset + refSize > dstSize) {
                refSize = dstSize - dstOffset;
            }
            u8 *dstPtr = dst + dstOffset;
            const u8 *refPtr = dst + refOffset;
            #pragma GCC unroll(8)
            for (u16 j = 0; j < refSize; j++) {
                *dstPtr++ = *refPtr++;
            }
            dstOffset += refSize;
        }
    }

    return dstOffset;
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
            }
            else {
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
