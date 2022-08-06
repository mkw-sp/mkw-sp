#pragma once

#include "revolution/gx/GXEnum.h"

typedef struct {
    u8 _00[0x80 - 0x00];
} GXFifoObj;

GXBool GXGetGPFifo(GXFifoObj *fifo);

void GXGetFifoPtrs(const GXFifoObj *fifo, void **readPtr, void **writePtr);

void GXEnableBreakPt(void *breakPtr);
void GXDisableBreakPt(void);
