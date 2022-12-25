#pragma once

#include "revolution/gx.h"

void TPLBind(void *tpl);
void *TPLGet(void *tpl, u32 id);
void TPLGetGXTexObjFromPalette(void *tpl, GXTexObj *obj, u32 id);
