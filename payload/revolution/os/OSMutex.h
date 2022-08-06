#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x18 - 0x00];
} OSMutex;
static_assert(sizeof(OSMutex) == 0x18);

void OSInitMutex(OSMutex *mutex);
void OSLockMutex(OSMutex *mutex);
void OSUnlockMutex(OSMutex *mutex);
BOOL OSTryLockMutex(OSMutex *mutex);
