#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
#define PLATFORM_EMULATOR
#endif

#define SP_DEBUG_STACK_RANDOMIZE (1 << 0)
#define SP_DEBUG_IOS_OPENS (1 << 1)
#define SP_DEBUG_LEVEL (SP_DEBUG_STACK_RANDOMIZE | SP_DEBUG_IOS_OPENS)

enum {
    kPlatform32 = 0,
    kPlatform64 = 1,

    kPlatformCurrent = sizeof(void *) == 8,
};

#define static_assert_32bit(s) static_assert(kPlatformCurrent != kPlatform32 || (s))

#ifdef PLATFORM_EMULATOR
#define PLATFORM_LE
#else
#define PLATFORM_BE
#endif

#ifdef __cplusplus
#define restrict __restrict
#endif

typedef int BOOL;

#ifndef PLATFORM_EMULATOR
// Some code relies on u32 being `%lu`
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
#else
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

typedef float f32;
typedef double f64;

// Source: https://stackoverflow.com/questions/34796571
#define ALIGNED_STRING(s)  (struct { alignas(0x20) char t[sizeof(s)]; }){ s }.t

#define MIN(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })
#define MAX(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

#define UNUSED(x) UNUSED_##x __attribute__((__unused__))

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member)))

//
// WARNING: BUILD_BUG_ON_ZERO / MUST_BE_ARRAY returns 4 on Windows.
//

#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int : (-!!(e)); })))
#define SAME_TYPE(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define MUST_BE_ARRAY(a) BUILD_BUG_ON_ZERO(SAME_TYPE((a), &(a)[0]))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + (MUST_BE_ARRAY(arr) && 0))

#define SP_TOSTRING(x) #x
#define SP_TOSTRING2(x) SP_TOSTRING(x)

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)

#ifdef _MSC_VER
#define PRAGMA_SECTION(s)
#else
#define PRAGMA_SECTION(s) __attribute__((section(s)))
#endif

#define CHANNEL_TITLE_ID UINT64_C(0x0001000153505350)
#define CHANNEL_TITLE_VERSION 0x0100
#define CHANNEL_CONTENT_COUNT 2

enum {
    BUILD_TYPE_DEBUG = 0,
    BUILD_TYPE_TEST = 1,
    BUILD_TYPE_RELEASE = 2,
};

typedef struct VersionInfo {
    u16 type;
    u16 major;
    u16 minor;
    u16 patch;
    u8 reserved[0x18];
    char name[0x20];
    char nickname[0x20];

#ifdef __cplusplus
    friend auto operator<=>(const VersionInfo &lhs, const VersionInfo &rhs);
#endif
} VersionInfo;

extern VersionInfo versionInfo;

#ifndef RVL_OS_NEEDS_IMPORT
#define RVL_OS_NEEDS_IMPORT \
    static_assert(false, "Please include revolution.h to use SP_LOG")
#endif

// clang: Merged May 16 2019, Clang 9
// GCC:   Merged May 20 2021, GCC 12 (likely to release April 2022)
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define SP_LOG(m, ...)                                                     \
    do {                                                                   \
        RVL_OS_NEEDS_IMPORT;                                               \
        OSReport("[" __FILE_NAME__ ":" SP_TOSTRING2(__LINE__) "] " m "\n", \
                ##__VA_ARGS__);                                            \
    } while (0)

static size_t sp_wcslen(const wchar_t *w) {
    size_t result = 0;
    while (*w++)
        ++result;
    return result;
}

#define VIRTUAL_TO_PHYSICAL(ptr) ((uintptr_t)(ptr) & 0x7fffffff)
#define PHYSICAL_TO_VIRTUAL(addr) ((void *)((addr) | 0x80000000))

#define ROUND_UP(n, a) (((uintptr_t)(n) + (a)-1) & ~((a)-1))

enum {
    REGION_P = 0x54a9,
    REGION_E = 0x5409,
    REGION_J = 0x53cd,
    REGION_K = 0x5511,
};

#define REGION (*(u16 *)0x8000620a)

#if !defined(NO_NEW_DELETE) && \
        !defined(__cplusplus)  // new/delete are reserved identifiers in C++
void *new (size_t size);

void delete (void *memBlock);
#endif

// payload/nw4r/lyt/lyt_pane.h:24:5: error: declaration does not declare anything [-fpermissive]
//    24 |     lyt_Pane_Base;
//       |     ^~
#ifdef __cplusplus
// lyt_Pane_Base base;
#define BASE(x) x base
#else
// lyt_Pane_Base;
#define BASE(x) x
#endif

typedef struct {
    f32 x;
    f32 y;
} Vec2;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

enum {
    PATCH_TYPE_WRITE = 0x0,
    PATCH_TYPE_BRANCH = 0x1,
};

typedef struct {
    u32 type;
    union {
        struct {
            void *dst;
            void *src;
            u32 size;
        } write;
        struct {
            void *from;
            void *to;
            bool link;
        } branch;
    };
} Patch;

#define PATCH_S16(function, offset, value) \
    __attribute__((section("patches"))) \
    extern const Patch patch_ ## function ## offset = { \
        .type = PATCH_TYPE_WRITE, \
        .write = { \
            (&function + offset), \
            &(s16) { (value) }, \
            sizeof(s16), \
        }, \
    }

#define PATCH_U32(function, offset, value) \
    __attribute__((section("patches"))) \
    extern const Patch patch_ ## function ## offset = { \
        .type = PATCH_TYPE_WRITE, \
        .write = { \
            (&function + offset), \
            &(u32) { (value) }, \
            sizeof(u32), \
        }, \
    }

#define PATCH_NOP(function, offset) PATCH_U32(function, offset, 0x60000000)

#define PATCH_BRANCH(from, to, link) \
    __attribute__((section("patches"))) \
    extern const Patch patch_ ## to = { \
        .type = PATCH_TYPE_BRANCH, \
        .branch = { \
            &from, \
            &to, \
            link, \
        }, \
    }

#define PATCH_B(from, to) \
    PATCH_BRANCH(from, to, false)

#define PATCH_BL(from, to) \
    PATCH_BRANCH(from, to, true)
