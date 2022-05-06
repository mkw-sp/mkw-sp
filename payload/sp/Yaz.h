#pragma once

#include <Common.h>

s32 Yaz_getDecodedSize(const u8 *src, u32 srcSize);

u32 Yaz_decode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize);

u32 Yaz_encode(const u8 *restrict src, u8 *restrict dst, u32 srcSize, u32 dstSize);
