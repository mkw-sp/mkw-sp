#pragma once

#include "revolution/gx/GXBump.h"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXFifo.h"
#include "revolution/gx/GXFrameBuffer.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXLighting.h"
#include "revolution/gx/GXManage.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXStruct.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTexture.h"

void GXSetAlphaUpdate(GXBool update_enable);
void GXSetClipMode(GXClipMode clip);
void GXSetCullMode(GXCullMode cull);
void GXSetZTexture(GXZTexOp op, GXTexFmt format, u32 bias);
void GXSetZMode(GXBool compare, GXCompare comparison, GXBool update);
void GXSetFog(GXFogType type, float start, float end, float near, float far,
        const GXColor *fogColor);
void GXSetProjection(const float mtx[4][4], GXProjectionType type);
void GXSetViewport(float x, float y, float width, float height, float near, float far);

void GXSetScissor(u32 left, u32 top, u32 right, u32 bottom);
void GXSetScissorBoxOffset(u32 x, u32 y);

void GXLoadTexObj(const GXTexObj *obj, GXTexMapID id);

void GXLoadPosMtxImm(const float mtx[3][4], u32);
void GXLoadNrmMtxImm(const f32 mtx[3][4], u32);
void GXSetCurrentMtx(u32);
void GXSetColorUpdate(u32);
void GXBegin(u32, u32, u32);
void GXSetChanAmbColor(GXChannelID, GXColor);
struct GXLightObj {
    u8 _00[0x0c - 0x00];
    GXColor color;
    f32 angle_attn[3];
    f32 dist_attn[3];
    f32 pos[3];
    f32 dir[3];
};
void GXLoadLightObjImm(struct GXLightObj *, int);

// bin_op(left_un_arg(ALPHA, left_un_arg), right_un_op(ALPHA, right_un_arg)
void GXSetAlphaCompare(GXCompare left_un_op, u8 left_un_arg, GXAlphaOp bin_op,
        GXCompare right_un_op, u8 right_un_arg);

static inline void GXEnd(void) {}
typedef union {
    char c;
    u8 _u8;
    s16 _s16;
    u16 _u16;
    s32 _s32;
    u32 _u32;
    int i;
    void *p;
    float f;
} WGPIPE_type;
#define WGPIPE (*(volatile WGPIPE_type *)0xcc008000)

static inline void GXColor1x8(u8 c) {
    WGPIPE._u8 = c;
}
static inline void GXColor1u32(u32 c) {
    WGPIPE._u32 = c;
}
static inline void GXPosition2s16(s16 a, s16 b) {
    WGPIPE._s16 = a;
    WGPIPE._s16 = b;
}
static inline void GXTexCoord2u16(u16 a, u16 b) {
    WGPIPE._u16 = a;
    WGPIPE._u16 = b;
}

static inline void GXPosition2f32(f32 a, f32 b) {
    WGPIPE.f = a;
    WGPIPE.f = b;
}
static inline void GXPosition3f32(f32 a, f32 b, f32 c) {
    WGPIPE.f = a;
    WGPIPE.f = b;
    WGPIPE.f = c;
}
static inline void GXTexCoord2f32(f32 a, f32 b) {
    WGPIPE.f = a;
    WGPIPE.f = b;
}

void GXCallDisplayList(const void *buf, u32 len);
