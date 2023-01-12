#pragma once

#include <revolution/os.h>

#define OS_THREAD_STACK_TOP_MAGIC 0xDEADBABE

typedef struct {
    u8 _0[0x8 - 0x0];
} OSThreadQueue;

typedef struct {
    u8 _000[0x2d8 - 0x000];
    void *val;
    u8 _2dc[0x308 - 0x2dc];
    u32 *stackTop;
    u8 _30c[0x318 - 0x30c];
} OSThread;
static_assert(sizeof(OSThread) == 0x318);

typedef void (*OSSwitchThreadCallback)(OSThread *from, OSThread *to);

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

OSSwitchThreadCallback OSSetSwitchThreadCallback(OSSwitchThreadCallback callback);

#define OSSleepMilliseconds(msec) OSSleepTicks(OSMillisecondsToTicks((OSTime)msec))
