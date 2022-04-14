#pragma once

#include "GXEnum.h"
#include "GXStruct.h"

void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c,
        GXTevColorArg d);

void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c,
        GXTevAlphaArg d);

void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp,
        GXTevRegID out_reg);

void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp,
        GXTevRegID out_reg);

void GXSetTevColor(GXTevRegID id, GXColor color);

void GXSetTevColorS10(GXTevRegID id, GXColorS10 color);

void GXSetNumTevStages(u8 nStages);

void GXSetTevKAlphaSel(GXTevStageID stage, GXTevKAlphaSel sel);
