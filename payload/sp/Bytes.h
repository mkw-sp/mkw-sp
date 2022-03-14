#pragma once

#include <Common.h>

static inline u8 read_u8(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0];
}

static inline u16 read_u16(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 8 | base[0x1];
}

static inline u32 read_u32(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x0] << 24 | base[0x1] << 16 | base[0x2] << 8 | base[0x3];
}

static inline u32 read_u32_le(const u8 *data, u32 offset) {
    const u8 *base = data + offset;
    return base[0x3] << 24 | base[0x2] << 16 | base[0x1] << 8 | base[0x0];
}

static inline void write_u8(u8 *data, u32 offset, u8 val) {
    u8 *base = data + offset;
    base[0x0] = val;
}

static inline void write_u16(u8 *data, u32 offset, u16 val) {
    u8 *base = data + offset;
    base[0x0] = val >> 8;
    base[0x1] = val;
}

static inline void write_u32(u8 *data, u32 offset, u32 val) {
    u8 *base = data + offset;
    base[0x0] = val >> 24;
    base[0x1] = val >> 16;
    base[0x2] = val >> 8;
    base[0x3] = val;
}

static inline void write_u32_le(u8 *data, u32 offset, u32 val) {
    u8 *base = data + offset;
    base[0x3] = val >> 24;
    base[0x2] = val >> 16;
    base[0x1] = val >> 8;
    base[0x0] = val;
}
