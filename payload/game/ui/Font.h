#pragma once

#include "../system/Mii.h"
#include <revolution.h>

#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/ut/ut_resFont.h>

typedef struct {
    lyt_TextBox *textBox;
    u32 formatId;
    u32 colorId;
    u8 _0c[0x10 - 0x0c];
} GlyphRenderer;
static_assert(sizeof(GlyphRenderer) == 0x10);

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
static inline s16 encodeSFixed6(float floating) {
    return (s16)(floating * (float)(1 << 5));
}

typedef struct {
    ut_ResFont base;
    GXTexObj *sheetTexObjs;
    u32 colorMode; // IA4, RGB5A3, I4
} Font;
static_assert(sizeof(Font) == 0x24);

Font *Font_ct(Font *self);

void Font_load(Font *self, const char *file);
void Font_calcQuad(const Font *self, RKFontGlyphQuad *result, u16 character);
