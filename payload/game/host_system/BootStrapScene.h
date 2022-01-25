#pragma once

#include <egg/core/eggHeap.h>

#include <revolution.h>

typedef struct {
    u8 _000[0x014 - 0x000];
    EGG_Heap *heapMem1;
    u8 _018[0xc70 - 0x018];
    void *relLoadThreadStack;
    OSThread *relLoadThread;
    u8 _c74[0xcd8 - 0xc78];
} BootStrapScene;
static_assert(sizeof(BootStrapScene) == 0xcd8);

void BootStrapScene_calc(BootStrapScene *this);

void BootStrapScene_draw(BootStrapScene *this);

void BootStrapScene_enter(BootStrapScene *this);

void BootStrapScene_exit(BootStrapScene *this);
