#pragma once

#include "revolution/gx/GXEnum.h"

void GXClearVtxDesc(void);
void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac);

void GXSetNumTexGens(u8 nTexGens);
