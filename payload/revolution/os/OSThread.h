#pragma once

#include <Common.h>

typedef struct OSThread OSThread;

struct OSThread {
    u8 _0[0x318];
};

BOOL OSCreateThread(OSThread *thread, void *(*func)(void *), void *param, void *stack,
    u32 stackSize, s32 priority, u16 attr);

BOOL OSJoinThread(OSThread *thread, void **val);

void OSDetachThread(OSThread *thread);

s32 OSResumeThread(OSThread *thread);

void OSSleepTicks(OSTime ticks);

#define OSSleepMilliseconds(msec) OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))
