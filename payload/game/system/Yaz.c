#include "Yaz.h"

enum {
    YAZ0_MAGIC = 0x59617a30,
    YAZ1_MAGIC = 0x59617a31,
};

static u8 getU8(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0];
}

static u16 getU16(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 8 | base[0x1];
}

static u32 getU32(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 24 | base[0x1] << 16 | base[0x2] << 8 | base[0x3];
}

s32 Yaz_getSize(const u8 *src) {
    u32 magic = getU32(src, 0x0);
    if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
        return -1;
    }

    return getU32(src, 0x4);
}

u32 Yaz_decode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize) {
    u32 magic = getU32(src, 0x0);
    if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
        return 0;
    }

    if (getU32(src, 0x4) < dstSize) {
        dstSize = getU32(src, 0x4);
    }

    u32 srcOffset = 0x10, dstOffset = 0x0;
    u8 groupHeader;
    for (u8 i = 0; srcOffset < srcSize && dstOffset < dstSize; i = (i + 1) % 8) {
        if (i == 0) {
            groupHeader = getU8(src, srcOffset);
            srcOffset++;
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
            u16 val = getU16(src, srcOffset);
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
                refSize = getU8(src, srcOffset) + 0x12;
                srcOffset++;
            }
            for (u16 j = 0; j < refSize; j++) {
                dst[dstOffset++] = dst[refOffset++];
                if (dstOffset == dstSize) {
                    return dstOffset;
                }
            }
        }
    }

    return dstOffset;
}
