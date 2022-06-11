#pragma once

#include <egg/core/eggHeap.h>

typedef struct {
    u8 _0000[0x0058 - 0x0000];
    u32 aspectRatio;
    u32 language;
    u8 _0060[0x1100 - 0x0060];
} SystemManager;
static_assert(sizeof(SystemManager) == 0x1100);

extern SystemManager *s_systemManager;

void SystemManager_shutdownSystem(SystemManager *self);
void my_SystemManager_shutdownSystem(SystemManager *self);
void SystemManager_returnToMenu(SystemManager *self);
void my_SystemManager_returnToMenu(SystemManager *self);
void SystemManager_restart(SystemManager *self);
void my_SystemManager_restart(SystemManager *self);

void *SystemManager_ripFromDisc(const char *path, EGG_Heap *heap, bool allocTop, u32 *size);
