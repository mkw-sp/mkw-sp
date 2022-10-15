#include "unitHeap.h"

void *REPLACED(MEMAllocFromUnitHeap)(MEMHeapHandle heap);
REPLACE void *MEMAllocFromUnitHeap(MEMHeapHandle heap) {
    void *memBlock = REPLACED(MEMAllocFromUnitHeap)(heap);
    if (!memBlock) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    return memBlock;
}
