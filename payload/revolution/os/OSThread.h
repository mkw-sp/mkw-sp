#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0x8 - 0x0];
} OSThreadQueue;

typedef struct {
    u8 _000[0x2d8 - 0x000];
    void *val;
    u8 _2dc[0x318 - 0x2dc];
} OSThread;
static_assert(sizeof(OSThread) == 0x318);

void OSInitThreadQueue(OSThreadQueue *queue);

OSThread *OSGetCurrentThread(void);

BOOL OSIsThreadTerminated(OSThread *thread);

s32 OSDisableScheduler(void);

s32 OSEnableScheduler(void);

BOOL OSCreateThread(OSThread *thread, void *(*func)(void *), void *param, void *stack,
        u32 stackSize, s32 priority, u16 attr);

BOOL OSJoinThread(OSThread *thread, void **val);

void OSDetachThread(OSThread *thread);

s32 OSResumeThread(OSThread *thread);

void OSSleepThread(OSThreadQueue *queue);

void OSWakeupThread(OSThreadQueue *queue);

void OSSleepTicks(OSTime ticks);

#define OSSleepMilliseconds(msec) OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))
