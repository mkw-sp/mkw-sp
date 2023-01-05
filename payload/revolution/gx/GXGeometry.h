#pragma once

#include "revolution/gx/GXEnum.h"

void GXClearVtxDesc(void);
void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac);
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GXSetNumTexGens(u8 nTexGens);
void GXSetTexCoordGen(GXTexCoordID texgen, GXTexGenType type, GXTexGenSrc src, u32 mtx);
void GXSetTexCoordGen2(GXTexCoordID texgen, GXTexGenType type, GXTexGenSrc src, u32 mtx,
        GXBool normalize, u32 postMtx);
