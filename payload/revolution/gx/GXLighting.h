#pragma once

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"

void GXSetChanMatColor(GXChannelID chan, GXColor mat_color);
void GXSetNumChans(u8 nChans);
void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src,
        u32 light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn);
