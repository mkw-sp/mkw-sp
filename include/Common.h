#pragma once

#include <sp/Panic.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SP_DEBUG_LEVEL_NONE (0 << 0)
#define SP_DEBUG_STACK_RANDOMIZE (1 << 0)
#define SP_DEBUG_LEVEL SP_DEBUG_LEVEL_NONE

enum {
    kPlatform32 = 0,
    kPlatform64 = 1,

    kPlatformCurrent = sizeof(void *) == 8,
};

#define static_assert_32bit(s) static_assert(kPlatformCurrent != kPlatform32 || (s))

#ifdef __cplusplus
#define restrict __restrict
#endif

typedef int BOOL;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define MIN(a, b) \
    ({ \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b; \
    })
#define MAX(a, b) \
    ({ \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b; \
    })

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

#define PRAGMA(s) _Pragma(s)

#define CHANNEL_TITLE_ID UINT64_C(0x0001000153505350)
#define CHANNEL_TITLE_VERSION 0x0300
#define CHANNEL_CONTENT_COUNT 3

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
} VersionInfo;

extern VersionInfo versionInfo;

__attribute__((format(printf, 1, 2))) static inline void CheckFormat(const char * /* msg */, ...) {}

#ifndef RVL_OS_NEEDS_IMPORT
#define RVL_OS_NEEDS_IMPORT static_assert(false, "Please include revolution.h to use SP_LOG")
#endif

// clang: Merged May 16 2019, Clang 9
// GCC:   Merged May 20 2021, GCC 12 (likely to release April 2022)
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define SP_LOG(m, ...) \
    do { \
        RVL_OS_NEEDS_IMPORT; \
        CheckFormat(m, ##__VA_ARGS__); \
        OSReport("[" __FILE_NAME__ ":" SP_TOSTRING2(__LINE__) "] " m "\n", ##__VA_ARGS__); \
    } while (0)

#define ROUND_UP(n, a) (((uintptr_t)(n) + (a)-1) & ~((a)-1))
#define ROUND_DOWN(n, a) ((uintptr_t)(n) & ~(a - 1))

enum {
    REGION_P = 0x54a9,
    REGION_E = 0x5409,
    REGION_J = 0x53cd,
    REGION_K = 0x5511,
};

#define REGION (*(u16 *)0x8000620a)

#if !defined(NO_NEW_DELETE) && !defined(__cplusplus) // new/delete are reserved identifiers in C++
void *new(size_t size);

void delete(void *memBlock);
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
            u32 *thunk;
        } branch;
    };
} Patch;

#define PATCH_S16(function, offset, value) \
    __attribute__((section("patches"))) const Patch patch_##function##offset = { \
            .type = PATCH_TYPE_WRITE, \
            .write = \
                    { \
                            (&function + offset), \
                            &(s16){(value)}, \
                            sizeof(s16), \
                    }, \
    }

#define PATCH_U32(function, offset, value) \
    __attribute__((section("patches"))) const Patch patch_##function##offset = { \
            .type = PATCH_TYPE_WRITE, \
            .write = \
                    { \
                            (&function + offset), \
                            &(u32){(value)}, \
                            sizeof(u32), \
                    }, \
    }

#define PATCH_NOP(function, offset) PATCH_U32(function, offset, 0x60000000)

#define PATCH_BRANCH(from, to, link, thunk) \
    __attribute__((section("patches"))) const Patch patch_##to = { \
            .type = PATCH_TYPE_BRANCH, \
            .branch = \
                    { \
                            (char *)(void *)(&from), \
                            (char *)(void *)(&to), \
                            link, \
                            thunk, \
                    }, \
    }

#define PATCH_B(from, to) PATCH_BRANCH(from, to, false, NULL)

#define PATCH_B_THUNK(from, to, thunk) PATCH_BRANCH(from, to, false, thunk)

#define PATCH_BL(from, to) PATCH_BRANCH(from, to, true, NULL)

#define REPLACE __attribute__((section("replacements")))
#define REPLACED(function) thunk_replaced_##function

#ifdef __clang__
#define __builtin_ppc_mftb() \
    ({ \
        u32 _rval; \
        asm volatile("mftb %0" : "=r"(_rval)); \
        _rval; \
    })
#endif
