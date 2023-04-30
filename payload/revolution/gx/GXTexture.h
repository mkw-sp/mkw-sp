#pragma once

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"

void GXInitTexObj(GXTexObj *obj, void *image_ptr, u16 width, u16 height, GXTexFmt format,
        GXTexWrapMode wrap_s, GXTexWrapMode wrap_t, GXBool mipmap);

void GXInvalidateTexAll(void);
