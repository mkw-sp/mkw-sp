#pragma once

#include <egg/core/eggHeap.h>

enum {
    SCENE_ID_MENU = 0x1,
    SCENE_ID_RACE = 0x2,
    SCENE_ID_GLOBE = 0x4,
};

enum {
    HEAP_ID_MEM1 = 0x0,
    HEAP_ID_MEM2 = 0x1,
    HEAP_ID_DEBUG = 0x2,
};

typedef struct {
    EGG_Heap *heaps[3];
    u8 _00c[0xc0c - 0x00c];
} HeapCollection;
static_assert(sizeof(HeapCollection) == 0xc0c);

EGG_Heap *HeapCollection_setGroupIdAll(HeapCollection *self, s32 groupId);
