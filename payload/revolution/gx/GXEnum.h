#pragma once

#include <Common.h>

typedef enum {
    GX_TF_I4 = 0x0,
    GX_TF_I8 = 0x1,
    GX_TF_IA4 = 0x2,
    GX_TF_IA8 = 0x3,
    GX_TF_RGB565 = 0x4,
    GX_TF_RGB5A3 = 0x5,
    GX_TF_RGBA8 = 0x6,
    GX_TF_CMPR = 0xE,
    GX_CTF_R4 = 0x20,
    GX_CTF_RA4 = 0x22,
    GX_CTF_RA8 = 0x23,
    GX_CTF_YUVA8 = 0x26,
    GX_CTF_A8 = 0x27,
    GX_CTF_R8 = 0x28,
    GX_CTF_G8 = 0x29,
    GX_CTF_B8 = 0x2A,
    GX_CTF_RG8 = 0x2B,
    GX_CTF_GB8 = 0x2C,
    GX_TF_Z8 = 0x11,
    GX_TF_Z16 = 0x13,
    GX_TF_Z24X8 = 0x16,
    GX_CTF_Z4 = 0x30,
    GX_CTF_Z8M = 0x39,
    GX_CTF_Z8L = 0x3A,
    GX_CTF_Z16L = 0x3C,
    GX_TF_A8 = 0x27,
} GXTexFmt;
static_assert(sizeof(GXTexFmt) == 0x4);

typedef enum { GX_ZT_DISABLE, GX_ZT_ADD, GX_ZT_REPLACE, GX_MAX_ZTEXOP } GXZTexOp;
static_assert(sizeof(GXZTexOp) == 4);

typedef enum { GX_CULL_NONE, GX_CULL_FRONT, GX_CULL_BACK, GX_CULL_ALL } GXCullMode;
static_assert(sizeof(GXCullMode) == 4);

typedef enum {
    GX_CLIP_ENABLE,
    GX_CLIP_DISABLE,
} GXClipMode;
static_assert(sizeof(GXClipMode) == 4);

typedef enum {
    GX_NEVER,
    GX_LESS,
    GX_EQUAL,
    GX_LEQUAL,
    GX_GREATER,
    GX_NEQUAL,
    GX_GEQUAL,
    GX_ALWAYS,
} GXCompare;

static_assert(sizeof(GXCompare) == 4);

typedef enum {
    GX_FOG_NONE = 0x00,

    GX_FOG_PERSP_LIN = 0x02,
    GX_FOG_PERSP_EXP = 0x04,
    GX_FOG_PERSP_EXP2 = 0x05,
    GX_FOG_PERSP_REVEXP = 0x06,
    GX_FOG_PERSP_REVEXP2 = 0x07,

    GX_FOG_ORTHO_LIN = 0x0A,
    GX_FOG_ORTHO_EXP = 0x0C,
    GX_FOG_ORTHO_EXP2 = 0x0D,
    GX_FOG_ORTHO_REVEXP = 0x0E,
    GX_FOG_ORTHO_REVEXP2 = 0x0F,
} GXFogType;
static_assert(sizeof(GXFogType) == 4);

typedef enum { GX_PERSPECTIVE, GX_ORTHOGRAPHIC } GXProjectionType;
static_assert(sizeof(GXProjectionType) == 4);

typedef enum {
    GX_FALSE,
    GX_TRUE,
} GXBool;
static_assert(sizeof(GXBool) == 4);
