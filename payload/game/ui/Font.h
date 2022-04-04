#pragma once

#include <revolution.h>
#include "../system/Mii.h"

#include <nw4r/ut/ut_resFont.h>

typedef struct {
    u8 gap0[0x84 - 0x0];
    f32 arrf3284[4];
    u8 gap94[0xb9 - 0x94];
    u8 unsignedB9;
    u8 gapBA[0xdc - 0xba];
    u8 byteDC;
    u8 byteDD;
    u8 byteDE;
    u8 unsignedDF;
} GlyphRenderer_sub0;
typedef struct {
    GlyphRenderer_sub0 *dword0;
    u32 dword4;
    u8 gap8[0x8 - 0x4];
    u32 dwordC;
    u8 gap10[0x14 - 0x10];
    int int14;
    int int18;
} GlyphRenderer;
void GlyphRenderer_setMaterial(GlyphRenderer *self);

typedef struct {
    u16 left;
    u16 right;
    u16 top;
    u16 bottom;
} RectU16;

typedef struct {
    GXTexObj *tex_obj;
    RectU16 uv;
} RKFontGlyphQuad;

static inline float decodeFixed15(u16 fixed) {
    return (float)(fixed) / (float)(1 << 15);
}
static inline u16 encodeFixed15(float floating) {
    return (u16)(floating * (float)(1 << 15));
}
static inline s16 encodeSFixed6(float floating) {
    return (s16)(floating * (float)(1 << 5));
}

typedef struct {
    ut_ResFont base;
    GXTexObj *sheetTexObjs;
    u32 colorMode;  // IA4, RGB5A3, I4
} Font;
static_assert(sizeof(Font) == 0x24);

Font *Font_ct(Font *self);
void Font_dt(Font *self);

void Font_load(Font *self, const char *file);
void Font_buildTexObjs(Font *self);
void Font_calcQuad(const Font *self, RKFontGlyphQuad *result, u16 character);

typedef struct {
    s32 intVals[9];
    u32 messageIds[9];
    Mii *miis[9];
    u8 _6c[0x9c - 0x6c];
    wchar_t *strings[9];
    u8 _c0[0xc4 - 0xc0];
} MessageInfo;
static_assert(sizeof(MessageInfo) == 0xc4);
