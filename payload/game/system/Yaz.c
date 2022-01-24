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

s32 Yaz_getSize(const u8 *src) {
    u32 magic = readU32(src, 0x0);
    if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
        return -1;
    }
    return readU32(src, 0x4);
}

u32 Yaz_decode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize) {
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
        u32 firstRefOffset = dstOffset < 0x1000 ? 0x0 : dstOffset - 0x1000;
        u32 bestRefSize = 0x1, bestRefOffset;
        for (u32 refOffset = firstRefOffset; refOffset < srcOffset; refOffset++) {
            u32 refSize;
            for (refSize = 0; refSize < 0x111 && dstOffset + refSize < dstSize; refSize++) {
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

enum {
    STATE_HEADER,
    STATE_GROUP_HEADER,
    STATE_COPY,
    STATE_REF_HEADER0,
    STATE_REF_HEADER1,
    STATE_REF_HEADER2,
    STATE_REF_COPY,
};

void YazDecoder_init(YazDecoder *this) {
    this->state = STATE_HEADER;
    this->groupHeaderIndex = 7;
    this->totalSrcSize = 0x0;
    this->totalDstSize = 0x0;
    this->expectedTotalDstSize = UINT32_MAX;
    this->bufferOffset = 0x0;
}

static u8 YazDecoder_read(YazDecoder *this, const u8 *restrict *src, u32 *srcSize) {
    u8 val = *(*src)++;
    (*srcSize)--;
    this->totalSrcSize++;
    return val;
}

static void YazDecoder_write(YazDecoder *this, u8 *restrict *dst, u32 *dstSize, u8 val) {
    *(*dst)++ = val;
    (*dstSize)--;
    this->totalDstSize++;
    this->buffer[this->bufferOffset++] = val;
    if (this->bufferOffset == 0x1000) {
        this->bufferOffset = 0x0;
    }
}

static bool YazDecoder_process(YazDecoder *this, const u8 *restrict *src, u8 *restrict *dst, u32 *srcSize, u32 *dstSize) {
    u8 val;
    switch (this->state) {
    case STATE_HEADER:
        this->header.buffer[this->totalSrcSize] = YazDecoder_read(this, src, srcSize);
        if (this->totalSrcSize == 0x10) {
            u32 magic = readU32(this->header.buffer, 0x0);
            if (magic != YAZ0_MAGIC && magic != YAZ1_MAGIC) {
                return false;
            }
            this->expectedTotalDstSize = readU32(this->header.buffer, 0x4);
            this->state = STATE_GROUP_HEADER;
        }
        return true;
    case STATE_GROUP_HEADER:
        this->groupHeaderIndex++;
        if (this->groupHeaderIndex == 8) {
            this->groupHeader = YazDecoder_read(this, src, srcSize);
            this->groupHeaderIndex = 0;
        }
        if (this->groupHeader >> (7 - this->groupHeaderIndex) & 1) {
            this->state = STATE_COPY;
        } else {
            this->state = STATE_REF_HEADER0;
        }
        return true;
    case STATE_COPY:
        val = YazDecoder_read(this, src, srcSize);
        YazDecoder_write(this, dst, dstSize, val);
        this->state = STATE_GROUP_HEADER;
        return true;
    case STATE_REF_HEADER0:
        val = YazDecoder_read(this, src, srcSize);
        this->ref.offset = val << 8 & 0xf00;
        this->ref.size = (val >> 4) + 0x2;
        this->state = STATE_REF_HEADER1;
        return true;
    case STATE_REF_HEADER1:
        val = YazDecoder_read(this, src, srcSize);
        this->ref.offset += val + 0x1;
        if (this->ref.offset > this->totalDstSize) {
            return false;
        }
        if (this->ref.size == 0x2) {
            this->state = STATE_REF_HEADER2;
        } else {
            this->state = STATE_REF_COPY;
        }
        return true;
    case STATE_REF_HEADER2:
        val = YazDecoder_read(this, src, srcSize);
        this->ref.size = val + 0x12;
        this->state = STATE_REF_COPY;
        return true;
    case STATE_REF_COPY:
        val = this->buffer[(this->bufferOffset - this->ref.offset) & 0xfff];
        YazDecoder_write(this, dst, dstSize, val);
        this->ref.size--;
        if (this->ref.size == 0x0) {
            this->state = STATE_GROUP_HEADER;
        }
        return true;
    default:
        // Should be unreachable
        return false;
    }
}

u32 YazDecoder_feed(YazDecoder *this, const u8 *restrict *src, u8 *restrict *dst, u32 *srcSize, u32 *dstSize) {
    do {
        if (this->totalDstSize == this->expectedTotalDstSize) {
            if (this->state == STATE_GROUP_HEADER) {
                return YAZ_DECODER_RESULT_DONE;
            } else {
                return YAZ_DECODER_RESULT_ERROR;
            }
        }

        switch (this->state) {
        case STATE_HEADER:
        case STATE_GROUP_HEADER:
        case STATE_REF_HEADER0:
        case STATE_REF_HEADER1:
        case STATE_REF_HEADER2:
            if (*srcSize == 0) {
                return YAZ_DECODER_RESULT_WANTS_SRC;
            }
            break;
        case STATE_COPY:
            if (*srcSize == 0) {
                return YAZ_DECODER_RESULT_WANTS_SRC;
            } else if (*dstSize == 0) {
                return YAZ_DECODER_RESULT_WANTS_DST;
            }
            break;
        case STATE_REF_COPY:
            if (*dstSize == 0) {
                return YAZ_DECODER_RESULT_WANTS_DST;
            }
            break;
        default:
            // Should be unreachable
            return YAZ_DECODER_RESULT_ERROR;
        }
    } while (YazDecoder_process(this, src, dst, srcSize, dstSize));

    return YAZ_DECODER_RESULT_ERROR;
}
