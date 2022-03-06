#pragma once

#include <Common.h>

s32 Yaz_getSize(const u8 *src);

u32 Yaz_decode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize);

u32 Yaz_encode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize);

typedef struct {
    u8 state;
    u8 groupHeaderIndex;
    u8 groupHeader;
    union {
        struct {
            u8 buffer[0x10];
        } header;
        struct {
            u16 size;
            u16 offset;
        } ref;
    };
    u32 totalSrcSize;
    u32 totalDstSize;
    u32 expectedTotalDstSize;
    u16 bufferOffset;
    u8 buffer[0x1000];
} YazDecoder;

enum {
    YAZ_DECODER_RESULT_ERROR,
    YAZ_DECODER_RESULT_DONE,
    YAZ_DECODER_RESULT_WANTS_SRC,
    YAZ_DECODER_RESULT_WANTS_DST,
};

void YazDecoder_init(YazDecoder *this);

u32 YazDecoder_feed(YazDecoder *this, const u8 *restrict *src, u8 *restrict *dst, u32 *srcSize,
        u32 *dstSize);
