#pragma once

#include "revolution/gx/GXStruct.h"

#ifdef RVL_OS_NEEDS_IMPORT
#undef RVL_OS_NEEDS_IMPORT
#define RVL_OS_NEEDS_IMPORT
#endif

void *OSGetMEM1ArenaHi(void);
void *OSGetMEM2ArenaHi(void);
void *OSGetMEM1ArenaLo(void);
void *OSGetMEM2ArenaLo(void);
void OSSetMEM1ArenaHi(void *hi);
void OSSetMEM2ArenaHi(void *hi);
void OSSetMEM1ArenaLo(void *lo);
void OSSetMEM2ArenaLo(void *lo);

static inline void *OSAllocFromMEM1ArenaHi(u32 size, u32 align) {
    uintptr_t result = ROUND_DOWN((uintptr_t)OSGetMEM1ArenaHi(), align);

    OSSetMEM1ArenaHi((void *)ROUND_DOWN(result - size, align));
    return (void *)result;
}

static inline void *OSAllocFromMEM2ArenaHi(u32 size, u32 align) {
    uintptr_t result = ROUND_DOWN((uintptr_t)OSGetMEM2ArenaHi(), align);

    OSSetMEM2ArenaHi((void *)ROUND_DOWN(result - size, align));
    return (void *)result;
}

void *OSAllocFromMEM1ArenaLo(u32 size, u32 align);

static inline void *OSAllocFromMEM2ArenaLo(u32 size, u32 align) {
    uintptr_t result = ROUND_UP((uintptr_t)OSGetMEM2ArenaLo(), align);

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

#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks(usec) ((usec) * (OS_TIMER_CLOCK / 1000000))
#define OSNanosecondsToTicks(nsec) ((nsec) / (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToNanoseconds(ticks) ((ticks) * (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

u32 OSGetTick(void);
OSTime OSGetTime(void);
void OSSetTime(OSTime time);

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

OSTime OSCalendarTimeToTicks(OSCalendarTime *time);
void OSTicksToCalendarTime(OSTime ticks, OSCalendarTime *td);

__attribute((format(printf, 1, 2))) void OSReport(const char *msg, ...);
void OSFatal(GXColor fg, GXColor bg, const char *msg);

const char* OSGetAppGamename(void);

#include "revolution/os/OSCache.h"
#include "revolution/os/OSMemory.h"
#include "revolution/os/OSMessage.h"
#include "revolution/os/OSModule.h"
#include "revolution/os/OSMutex.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSThread.h"
#include "revolution/os/OSTitle.h"

extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32);
