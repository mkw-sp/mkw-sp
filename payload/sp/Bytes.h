#pragma once

#include <Common.h>

static inline u32 swap32(u32 v);
static inline u16 swap16(u16 v);

#define _BSWAP_32(v)                                                                \
    (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) | ((v & 0x0000ff00) << 8) | \
            ((v & 0x000000ff) << 24))
#define _BSWAP_16(v) (((v & 0xff00) >> 8) | ((v & 0x00ff) << 8))

#if defined(__llvm__) || (defined(__GNUC__) && !defined(__ICC))
static inline u32 swap32(u32 v) {
    return __builtin_bswap32(v);
}
static inline u16 swap16(u16 v) {
    return _BSWAP_16(v);
}
#elif defined(_MSC_VER)
#include <stdlib.h>
static inline u32 swap32(u32 v) {
    return _byteswap_ulong(v);
}
static inline u16 swap16(u16 v) {
    return _byteswap_ushort(v);
}
#else
static inline u32 swap32(u32 v) {
    return _BSWAP_32(v);
}
static inline u16 swap16(u16 v) {
    return _BSWAP_16(v);
}
#endif

#ifdef PLATFORM_LE
#include <string.h>

// Respects strong aliasing, will never double evaluate
#define sp_htonl(x)                              \
    ({                                           \
        __typeof__(x) _x = x;                    \
        u32 _u;                                  \
        static_assert(sizeof(_x) == sizeof(_u)); \
        memcpy(&_u, &_x, sizeof(_x));            \
        _u = swap32(_u);                         \
        memcpy(&_x, &_u, sizeof(_x));            \
        _x;                                      \
    })
#define sp_htons(x)                              \
    ({                                           \
        __typeof__(x) _x = x;                    \
        u16 _u;                                  \
        static_assert(sizeof(_x) == sizeof(_u)); \
        memcpy(&_u, &_x, sizeof(_x));            \
        _u = swap16(_u);                         \
        memcpy(&_x, &_u, sizeof(_x));            \
        _x;                                      \
    })
#else
#define sp_htonl(x) (x)
#define sp_htons(x) (x)
#endif

#define sp_ntohl(x) sp_htonl(x)
#define sp_ntohs(x) sp_htons(x)

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
