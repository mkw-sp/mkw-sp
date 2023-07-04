#pragma once

#include "revolution/gx/GXStruct.h"

void GXSetCopyClear(GXColor clear_clr, u32 clear_z);
void GXSetCopyFilter(GXBool aa, const u8 sample_pattern[12][2], GXBool vf, const u8 vfilter[7]);
