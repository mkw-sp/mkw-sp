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
    GX_TEVSTAGE0,
    GX_TEVSTAGE1,
    GX_TEVSTAGE2,
    GX_TEVSTAGE3,
    GX_TEVSTAGE4,
    GX_TEVSTAGE5,
    GX_TEVSTAGE6,
    GX_TEVSTAGE7,
    GX_TEVSTAGE8,
    GX_TEVSTAGE9,
    GX_TEVSTAGE10,
    GX_TEVSTAGE11,
    GX_TEVSTAGE12,
    GX_TEVSTAGE13,
    GX_TEVSTAGE14,
    GX_TEVSTAGE15,
    GX_MAX_TEVSTAGE,
} GXTevStageID;
static_assert(sizeof(GXTevStageID) == 4);

typedef enum {
    GX_TEVPREV,
    GX_TEVREG0,
    GX_TEVREG1,
    GX_TEVREG2,
    GX_MAX_TEVREG,
} GXTevRegID;
static_assert(sizeof(GXTevRegID) == 4);

typedef enum {
    GX_TEV_ADD = 0,
    GX_TEV_SUB = 1,
    GX_TEV_COMP_R8_GT = 8,
    GX_TEV_COMP_R8_EQ = 9,
    GX_TEV_COMP_GR16_GT = 10,
    GX_TEV_COMP_GR16_EQ = 11,
    GX_TEV_COMP_BGR24_GT = 12,
    GX_TEV_COMP_BGR24_EQ = 13,
    GX_TEV_COMP_RGB8_GT = 14,
    GX_TEV_COMP_RGB8_EQ = 15,
    GX_TEV_COMP_A8_GT = GX_TEV_COMP_RGB8_GT,
    GX_TEV_COMP_A8_EQ = GX_TEV_COMP_RGB8_EQ,
} GXTevOp;
static_assert(sizeof(GXTevOp) == 4);

typedef enum {
    GX_CC_CPREV,
    GX_CC_APREV,
    GX_CC_C0,
    GX_CC_A0,
    GX_CC_C1,
    GX_CC_A1,
    GX_CC_C2,
    GX_CC_A2,
    GX_CC_TEXC,
    GX_CC_TEXA,
    GX_CC_RASC,
    GX_CC_RASA,
    GX_CC_ONE,
    GX_CC_HALF,
    GX_CC_KONST,
    GX_CC_ZERO,
} GXTevColorArg;
static_assert(sizeof(GXTevColorArg) == 4);

typedef enum {
    GX_CA_APREV,
    GX_CA_A0,
    GX_CA_A1,
    GX_CA_A2,
    GX_CA_TEXA,
    GX_CA_RASA,
    GX_CA_KONST,
    GX_CA_ZERO,
} GXTevAlphaArg;
static_assert(sizeof(GXTevAlphaArg) == 4);

typedef enum {
    GX_TB_ZERO,
    GX_TB_ADDHALF,
    GX_TB_SUBHALF,
    GX_MAX_TEVBIAS,
} GXTevBias;
static_assert(sizeof(GXTevBias) == 4);

typedef enum {
    GX_TEV_KCSEL_8_8 = 0x00,
    GX_TEV_KCSEL_7_8 = 0x01,
    GX_TEV_KCSEL_6_8 = 0x02,
    GX_TEV_KCSEL_5_8 = 0x03,
    GX_TEV_KCSEL_4_8 = 0x04,
    GX_TEV_KCSEL_3_8 = 0x05,
    GX_TEV_KCSEL_2_8 = 0x06,
    GX_TEV_KCSEL_1_8 = 0x07,
    GX_TEV_KCSEL_1 = GX_TEV_KCSEL_8_8,
    GX_TEV_KCSEL_3_4 = GX_TEV_KCSEL_6_8,
    GX_TEV_KCSEL_1_2 = GX_TEV_KCSEL_4_8,
    GX_TEV_KCSEL_1_4 = GX_TEV_KCSEL_2_8,
    GX_TEV_KCSEL_K0 = 0x0C,
    GX_TEV_KCSEL_K1 = 0x0D,
    GX_TEV_KCSEL_K2 = 0x0E,
    GX_TEV_KCSEL_K3 = 0x0F,
    GX_TEV_KCSEL_K0_R = 0x10,
    GX_TEV_KCSEL_K1_R = 0x11,
    GX_TEV_KCSEL_K2_R = 0x12,
    GX_TEV_KCSEL_K3_R = 0x13,
    GX_TEV_KCSEL_K0_G = 0x14,
    GX_TEV_KCSEL_K1_G = 0x15,
    GX_TEV_KCSEL_K2_G = 0x16,
    GX_TEV_KCSEL_K3_G = 0x17,
    GX_TEV_KCSEL_K0_B = 0x18,
    GX_TEV_KCSEL_K1_B = 0x19,
    GX_TEV_KCSEL_K2_B = 0x1A,
    GX_TEV_KCSEL_K3_B = 0x1B,
    GX_TEV_KCSEL_K0_A = 0x1C,
    GX_TEV_KCSEL_K1_A = 0x1D,
    GX_TEV_KCSEL_K2_A = 0x1E,
    GX_TEV_KCSEL_K3_A = 0x1F,
} GXTevKColorSel;
static_assert(sizeof(GXTevKColorSel) == 4);

typedef enum {
    GX_TEV_KASEL_8_8 = 0x00,
    GX_TEV_KASEL_7_8 = 0x01,
    GX_TEV_KASEL_6_8 = 0x02,
    GX_TEV_KASEL_5_8 = 0x03,
    GX_TEV_KASEL_4_8 = 0x04,
    GX_TEV_KASEL_3_8 = 0x05,
    GX_TEV_KASEL_2_8 = 0x06,
    GX_TEV_KASEL_1_8 = 0x07,
    GX_TEV_KASEL_1 = GX_TEV_KASEL_8_8,
    GX_TEV_KASEL_3_4 = GX_TEV_KASEL_6_8,
    GX_TEV_KASEL_1_2 = GX_TEV_KASEL_4_8,
    GX_TEV_KASEL_1_4 = GX_TEV_KASEL_2_8,
    GX_TEV_KASEL_K0_R = 0x10,
    GX_TEV_KASEL_K1_R = 0x11,
    GX_TEV_KASEL_K2_R = 0x12,
    GX_TEV_KASEL_K3_R = 0x13,
    GX_TEV_KASEL_K0_G = 0x14,
    GX_TEV_KASEL_K1_G = 0x15,
    GX_TEV_KASEL_K2_G = 0x16,
    GX_TEV_KASEL_K3_G = 0x17,
    GX_TEV_KASEL_K0_B = 0x18,
    GX_TEV_KASEL_K1_B = 0x19,
    GX_TEV_KASEL_K2_B = 0x1A,
    GX_TEV_KASEL_K3_B = 0x1B,
    GX_TEV_KASEL_K0_A = 0x1C,
    GX_TEV_KASEL_K1_A = 0x1D,
    GX_TEV_KASEL_K2_A = 0x1E,
    GX_TEV_KASEL_K3_A = 0x1F,
} GXTevKAlphaSel;
static_assert(sizeof(GXTevKAlphaSel) == 4);

typedef enum {
    GX_CS_SCALE_1,
    GX_CS_SCALE_2,
    GX_CS_SCALE_4,
    GX_CS_DIVIDE_2,
    GX_MAX_TEVSCALE,
} GXTevScale;
static_assert(sizeof(GXTevScale) == 4);

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
