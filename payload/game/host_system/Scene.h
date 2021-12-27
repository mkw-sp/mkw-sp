#pragma once

#include <egg/core/eggHeap.h>

enum {
    HEAP_ID_MEM1 = 0x0,
    HEAP_ID_MEM2 = 0x1,
    HEAP_ID_DEBUG = 0x2,
};

typedef struct {
    EGG_Heap *heaps[3];
    u8 _00c[0xc0c - 0x00c];
} HeapCollection;
