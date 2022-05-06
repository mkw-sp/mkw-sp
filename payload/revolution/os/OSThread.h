#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0x8 - 0x0];
} OSThreadQueue;

typedef struct {
    u8 _000[0x318 - 0x000];
} OSThread;
static_assert(sizeof(OSThread) == 0x318);

void OSInitThreadQueue(OSThreadQueue *queue);

OSThread *OSGetCurrentThread(void);

BOOL OSCreateThread(OSThread *thread, void *(*func)(void *), void *param, void *stack,
        u32 stackSize, s32 priority, u16 attr);

BOOL OSJoinThread(OSThread *thread, void **val);

void OSDetachThread(OSThread *thread);

s32 OSResumeThread(OSThread *thread);

void OSSleepThread(OSThreadQueue *queue);

void OSWakeupThread(OSThreadQueue *queue);

void OSSleepTicks(OSTime ticks);

#define OSSleepMilliseconds(msec) OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))
