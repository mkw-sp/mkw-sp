#pragma once

#include <Common.h>

/*
static inline
u32 value_or_null(u32 val, bool cond) {
        // 1 -> 0xFFFF'FFFF
        // 0 -> 0x0000'0000
        u32 mask = ~(static_cast<u32>(cond) - 1);
        return val & mask;
}
*/
#define value_or_null(val, cond) ((val) & (~(((u32)(cond)) - 1)))

// is val in [low, high]
static inline bool in_range_inclusive(u32 val, u32 low, u32 high) {
    return (val - low) <= (high - low);
}

static inline u32 parse_hex32(const char *begin, const char *end) {
    u32 hex = 0;
    for (const char *i = begin; i < end && i < begin + 8; ++i) {
        s32 c = *i;
        s32 current = 0;

        const bool is_upper = in_range_inclusive(c, 'A', 'F');
        current |= value_or_null(c - 'A' + 0xA, is_upper);

        const bool is_lower = in_range_inclusive(c, 'a', 'f');
        current |= value_or_null(c - 'a' + 0xA, is_lower);

        const bool is_num = in_range_inclusive(c, '0', '9');
        current |= value_or_null(c - '0', is_num);

        // current & 0xf; we don't have to mask off, since we know it is either 0 or
        // a valid number
        hex = (hex << 4) | current;
    }

    return hex;
}