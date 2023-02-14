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
    GX_KCOLOR0,
    GX_KCOLOR1,
    GX_KCOLOR2,
    GX_KCOLOR3,
    GX_MAX_KCOLOR,
} GXTevKColorID;
static_assert(sizeof(GXTevKColorID) == 4);

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

typedef enum {
    GX_POINTS = 0xb8,
    GX_LINES = 0xa8,
    GX_LINESTRIP = 0xb0,
    GX_TRIANGLES = 0x90,
    GX_TRIANGLESTRIP = 0x98,
    GX_TRIANGLEFAN = 0xa0,
    GX_QUADS = 0x80,
} GXPrimitive;
static_assert(sizeof(GXPrimitive) == 4);

typedef enum {
    GX_VA_PNMTXIDX = 0,
    GX_VA_TEX0MTXIDX,
    GX_VA_TEX1MTXIDX,
    GX_VA_TEX2MTXIDX,
    GX_VA_TEX3MTXIDX,
    GX_VA_TEX4MTXIDX,
    GX_VA_TEX5MTXIDX,
    GX_VA_TEX6MTXIDX,
    GX_VA_TEX7MTXIDX,
    GX_VA_POS,
    GX_VA_NRM,
    GX_VA_CLR0,
    GX_VA_CLR1,
    GX_VA_TEX0,
    GX_VA_TEX1,
    GX_VA_TEX2,
    GX_VA_TEX3,
    GX_VA_TEX4,
    GX_VA_TEX5,
    GX_VA_TEX6,
    GX_VA_TEX7,
    GX_POS_MTX_ARRAY,
    GX_NRM_MTX_ARRAY,
    GX_TEX_MTX_ARRAY,
    GX_LIGHT_ARRAY,
    GX_VA_NBT,
    GX_VA_MAX_ATTR,
    GX_VA_NULL = 0xff,
} GXAttr;
static_assert(sizeof(GXAttr) == 4);

typedef enum {
    GX_VTXFMT0,
    GX_VTXFMT1,
    GX_VTXFMT2,
    GX_VTXFMT3,
    GX_VTXFMT4,
    GX_VTXFMT5,
    GX_VTXFMT6,
    GX_VTXFMT7,
    GX_MAX_VTXFMT,
} GXVtxFmt;
static_assert(sizeof(GXVtxFmt) == 4);

typedef enum {
    GX_TEXCOORD0,
    GX_TEXCOORD1,
    GX_TEXCOORD2,
    GX_TEXCOORD3,
    GX_TEXCOORD4,
    GX_TEXCOORD5,
    GX_TEXCOORD6,
    GX_TEXCOORD7,
    GX_MAX_TEXCOORD,
    GX_TEXCOORD_NULL = 0xff,
} GXTexCoordID;
static_assert(sizeof(GXTexCoordID) == 4);

typedef enum {
    GX_TG_MTX3x4,
    GX_TG_MTX2x4,
    GX_TG_BUMP0,
    GX_TG_BUMP1,
    GX_TG_BUMP2,
    GX_TG_BUMP3,
    GX_TG_BUMP4,
    GX_TG_BUMP5,
    GX_TG_BUMP6,
    GX_TG_BUMP7,
    GX_TG_SRTG,
} GXTexGenType;
static_assert(sizeof(GXTexGenType) == 4);

typedef enum {
    GX_TG_POS,
    GX_TG_NRM,
    GX_TG_BINRM,
    GX_TG_TANGENT,
    GX_TG_TEX0,
    GX_TG_TEX1,
    GX_TG_TEX2,
    GX_TG_TEX3,
    GX_TG_TEX4,
    GX_TG_TEX5,
    GX_TG_TEX6,
    GX_TG_TEX7,
    GX_TG_TEXCOORD0,
    GX_TG_TEXCOORD1,
    GX_TG_TEXCOORD2,
    GX_TG_TEXCOORD3,
    GX_TG_TEXCOORD4,
    GX_TG_TEXCOORD5,
    GX_TG_TEXCOORD6,
    GX_TG_COLOR0,
    GX_TG_COLOR1
} GXTexGenSrc;
static_assert(sizeof(GXTexGenSrc) == 4);

typedef enum {
    GX_TEXMTX0 = 30,
    GX_TEXMTX1 = 33,
    GX_TEXMTX2 = 36,
    GX_TEXMTX3 = 39,
    GX_TEXMTX4 = 42,
    GX_TEXMTX5 = 45,
    GX_TEXMTX6 = 48,
    GX_TEXMTX7 = 51,
    GX_TEXMTX8 = 54,
    GX_TEXMTX9 = 57,
    GX_IDENTITY = 60,
} GXTexMtx;
static_assert(sizeof(GXTexMtx) == 4);

typedef enum {
    GX_PTTEXMTX0 = 64,
    GX_PTTEXMTX1 = 67,
    GX_PTTEXMTX2 = 70,
    GX_PTTEXMTX3 = 73,
    GX_PTTEXMTX4 = 76,
    GX_PTTEXMTX5 = 79,
    GX_PTTEXMTX6 = 82,
    GX_PTTEXMTX7 = 85,
    GX_PTTEXMTX8 = 88,
    GX_PTTEXMTX9 = 91,
    GX_PTTEXMTX10 = 94,
    GX_PTTEXMTX11 = 97,
    GX_PTTEXMTX12 = 100,
    GX_PTTEXMTX13 = 103,
    GX_PTTEXMTX14 = 106,
    GX_PTTEXMTX15 = 109,
    GX_PTTEXMTX16 = 112,
    GX_PTTEXMTX17 = 115,
    GX_PTTEXMTX18 = 118,
    GX_PTTEXMTX19 = 121,
    GX_PTIDENTITY = 125,
} GXPTTexMtx;
static_assert(sizeof(GXPTTexMtx) == 4);

typedef enum {
    GX_POS_XY = 0,
    GX_POS_XYZ = 1,
    GX_NRM_XYZ = 0,
    GX_NRM_NBT = 1,
    GX_NRM_NBT3 = 2,
    GX_CLR_RGB = 0,
    GX_CLR_RGBA = 1,
    GX_TEX_S = 0,
    GX_TEX_ST = 1,
} GXCompCnt;
static_assert(sizeof(GXCompCnt) == 4);

typedef enum {
    GX_U8 = 0,
    GX_S8 = 1,
    GX_U16 = 2,
    GX_S16 = 3,
    GX_F32 = 4,
    GX_RGB565 = 0,
    GX_RGB8 = 1,
    GX_RGBX8 = 2,
    GX_RGBA4 = 3,
    GX_RGBA6 = 4,
    GX_RGBA8 = 5,
} GXCompType;
static_assert(sizeof(GXCompType) == 4);

typedef enum {
    GX_COLOR0,
    GX_COLOR1,
    GX_ALPHA0,
    GX_ALPHA1,
    GX_COLOR0A0,
    GX_COLOR1A1,
    GX_COLOR_ZERO,
    GX_ALPHA_BUMP,
    GX_ALPHA_BUMPN,
    GX_COLOR_NULL = 0xff,
} GXChannelID;
static_assert(sizeof(GXChannelID) == 4);

typedef enum {
    GX_SRC_REG,
    GX_SRC_VTX,
} GXColorSrc;
static_assert(sizeof(GXColorSrc) == 4);

typedef enum {
    GX_DF_NONE,
    GX_DF_SIGN,
    GX_DF_CLAMP,
} GXDiffuseFn;
static_assert(sizeof(GXDiffuseFn) == 4);

typedef enum {
    GX_AF_SPEC,
    GX_AF_SPOT,
    GX_AF_NONE,
} GXAttnFn;
static_assert(sizeof(GXAttnFn) == 4);

typedef enum {
    GX_CLAMP,
    GX_REPEAT,
    GX_MIRROR,
    GX_MAX_TEXWRAPMODE,
} GXTexWrapMode;
static_assert(sizeof(GXTexWrapMode) == 4);

typedef enum {
    GX_TEXMAP0,
    GX_TEXMAP1,
    GX_TEXMAP2,
    GX_TEXMAP3,
    GX_TEXMAP4,
    GX_TEXMAP5,
    GX_TEXMAP6,
    GX_TEXMAP7,
    GX_MAX_TEXMAP,
    GX_TEXMAP_NULL = 0xff,
    GX_TEX_DISABLE = 0x100,
} GXTexMapID;
static_assert(sizeof(GXTexMapID) == 4);

typedef enum {
    GX_AOP_AND,
    GX_AOP_OR,
    GX_AOP_XOR,
    GX_AOP_XNOR,
    GX_MAX_ALPHAOP,
} GXAlphaOp;
static_assert(sizeof(GXAlphaOp) == 4);

typedef enum {
    GX_BM_NONE,
    GX_BM_BLEND,
    GX_BM_LOGIC,
    GX_BM_SUBTRACT,
    GX_MAX_BLENDMODE,
} GXBlendMode;
static_assert(sizeof(GXBlendMode) == 4);

typedef enum {
    GX_BL_ZERO,
    GX_BL_ONE,
    GX_BL_SRCCLR,
    GX_BL_INVSRCCLR,
    GX_BL_SRCALPHA,
    GX_BL_INVSRCALPHA,
    GX_BL_DSTALPHA,
    GX_BL_INVDSTALPHA,
    GX_BL_DSTCLR = GX_BL_SRCCLR,
    GX_BL_INVDSTCLR = GX_BL_INVSRCCLR,
} GXBlendFactor;
static_assert(sizeof(GXBlendFactor) == 4);

typedef enum {
    GX_LO_CLEAR,
    GX_LO_AND,
    GX_LO_REVAND,
    GX_LO_COPY,
    GX_LO_INVAND,
    GX_LO_NOOP,
    GX_LO_XOR,
    GX_LO_OR,
    GX_LO_NOR,
    GX_LO_EQUIV,
    GX_LO_INV,
    GX_LO_REVOR,
    GX_LO_INVCOPY,
    GX_LO_INVOR,
    GX_LO_NAND,
    GX_LO_SET,
} GXLogicOp;
static_assert(sizeof(GXLogicOp) == 4);

typedef enum {
    GX_MODULATE,
    GX_DECAL,
    GX_BLEND,
    GX_REPLACE,
    GX_PASSCLR,
} GXTevMode;
static_assert(sizeof(GXTevMode) == 4);

typedef enum {
    GX_NONE,
    GX_DIRECT,
    GX_INDEX8,
    GX_INDEX16,
} GXAttrType;
static_assert(sizeof(GXAttrType) == 4);
