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
