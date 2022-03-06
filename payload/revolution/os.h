#pragma once

#include "revolution/gx/GXStruct.h"

#ifdef RVL_OS_NEEDS_IMPORT
#undef RVL_OS_NEEDS_IMPORT
#define RVL_OS_NEEDS_IMPORT
#endif

void *OSGetMEM1ArenaHi(void);
void *OSGetMEM1ArenaLo(void);

void *OSGetMEM2ArenaLo(void);
void OSSetMEM2ArenaLo(void *lo);

void *OSAllocFromMEM1ArenaLo(u32 size, u32 align);

static inline void *OSAllocFromMEM2ArenaLo(u32 size, u32 align) {
    u32 result = ROUND_UP((u32)OSGetMEM2ArenaLo(), align);

    OSSetMEM2ArenaLo((void *)ROUND_UP(result + size, align));
    return (void *)result;
}

u32 OSGetPhysicalMem1Size(void);
u32 OSGetPhysicalMem2Size(void);

#define OS_MEM1_REGION_PREFIX 0x00000000
#define OS_MEM2_REGION_PREFIX 0x10000000
#define OS_MEM_REGION_ID_MASK 0x30000000
#define OSIsMEM1Region(addr) \
    (((u32)(addr)&OS_MEM_REGION_ID_MASK) == OS_MEM1_REGION_PREFIX)
#define OSIsMEM2Region(addr) \
    (((u32)(addr)&OS_MEM_REGION_ID_MASK) == OS_MEM2_REGION_PREFIX)

void *OSGetStackPointer(void);

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))

void OSInit(void);

typedef s64 OSTime;

#define OS_BUS_CLOCK (*(u32 *)0x800000f8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToNanoseconds(ticks) ((ticks) * (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

u32 OSGetTick(void);

OSTime OSGetTime(void);

typedef struct {
    int sec;
    int min;
    int hour;
    int mday;
    int mon;
    int year;
    int wday;
    int yday;

    int msec;
    int usec;
} OSCalendarTime;

void OSTicksToCalendarTime(OSTime ticks, OSCalendarTime *td);

void OSReport(const char *msg, ...);
void OSFatal(GXColor fg, GXColor bg, const char *msg);

#include "revolution/os/OSCache.h"
#include "revolution/os/OSMemory.h"
#include "revolution/os/OSModule.h"
#include "revolution/os/OSMutex.h"
#include "revolution/os/OSThread.h"
