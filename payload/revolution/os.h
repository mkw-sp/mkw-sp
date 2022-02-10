#pragma once

#include "revolution/gx/GXStruct.h"

void *OSAllocFromMEM1ArenaLo(u32 size, u32 align);

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))

void OSInit(void);

typedef s64 OSTime;

#define OS_BUS_CLOCK (*(u32 *)0x800000f8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

void OSReport(const char *msg, ...);
void OSFatal(GXColor fg, GXColor bg, const char *msg);

#include "revolution/os/OSCache.h"
#include "revolution/os/OSContext.h"
#include "revolution/os/OSError.h"
#include "revolution/os/OSMemory.h"
#include "revolution/os/OSModule.h"
#include "revolution/os/OSThread.h"
