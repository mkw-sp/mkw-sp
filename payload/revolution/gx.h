#pragma once

#include <revolution/gx/GXEnum.h>
#include <revolution/gx/GXStruct.h>
#include <revolution/gx/GXTev.h>

void GXSetClipMode(GXClipMode clip);
void GXSetCullMode(GXCullMode cull);
void GXSetZTexture(GXZTexOp op, GXTexFmt format, u32 bias);
void GXSetZMode(GXBool compare, GXCompare comparison, GXBool update);
void GXSetFog(GXFogType type, float start, float end, float near, float far,
        const GXColor *fogColor);
void GXSetProjection(const float *mtx44, GXProjectionType type);
void GXSetViewport(float x, float y, float width, float height, float near, float far);

void GXSetScissor(u32 left, u32 top, u32 right, u32 bottom);
void GXSetScissorBoxOffset(u32 x, u32 y);

void GXLoadTexObj(const GXTexObj *obj, GXTexMapID id);

void GXLoadPosMtxImm(float *, u32);
void GXSetCurrentMtx(u32);
void GXSetColorUpdate(u32);
void GXBegin(u32, u32, u32);

// bin_op(left_un_arg(ALPHA, left_un_arg), right_un_op(ALPHA, right_un_arg)
void GXSetAlphaCompare(GXCompare left_un_op, u8 left_un_arg, GXAlphaOp bin_op, GXCompare right_un_op, u8 right_un_arg);

static inline void GXEnd(void) {}
typedef union {
    char c;
    s16 _s16;
    u16 _u16;
    int i;
    void *p;
    float f;
} WGPIPE_type;
#define WGPIPE (*(volatile WGPIPE_type *)0xcc008000)

static inline void GXPosition2s16(s16 a, s16 b) {
    WGPIPE._s16 = a;
    WGPIPE._s16 = b;
}
static inline void GXTexCoord2u16(u16 a, u16 b) {
    WGPIPE._u16 = a;
    WGPIPE._u16 = b;
}
