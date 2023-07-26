#pragma once

#include "revolution/gx.h"

typedef struct {
    u32 versionNumber;
    u32 numImageDescriptors;
    void *imageDescriptorArray;
} TPLPalette;
static_assert(sizeof(TPLPalette) == 0xC);

void REPLACED(TPLBind)(TPLPalette *tpl);
REPLACE void TPLBind(TPLPalette *tpl);
void *TPLGet(TPLPalette *tpl, u32 id);
void TPLGetGXTexObjFromPalette(TPLPalette *tpl, GXTexObj *obj, u32 id);
