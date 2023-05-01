#pragma once

#include <Common.h>
#include <string.h>
#include <wchar.h>

static inline u32 Util_toUtf16(wchar_t *dst, u32 dst_max, const char *src, u32 src_max) {
    const u32 min_bound = MIN(dst_max, src_max);
    const u32 len = strnlen(src, min_bound);

    for (u32 i = 0; i < len; ++i) {
        dst[i] = (wchar_t)src[i];
    }

    return len;
}
static inline u32 Util_toUtf8(char *dst, u32 dst_max, const wchar_t *src, u32 src_max) {
    const u32 min_bound = MIN(dst_max, src_max);
    const u32 len = wcslen(src /*, min_bound*/);

    for (u32 i = 0; i < MIN(min_bound, len); ++i) {
        dst[i] = (char)src[i];
    }

    return len;
}
