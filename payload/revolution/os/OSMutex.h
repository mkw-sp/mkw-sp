#pragma once

typedef struct {
  char _[0x18];
} OSMutex;

void OSInitMutex(OSMutex* mutex);
void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);
