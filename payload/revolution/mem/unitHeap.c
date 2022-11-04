#include "unitHeap.h"

MEMHeapHandle REPLACED(
        MEMCreateUnitHeapEx)(void *startAddress, u32 size, u32 blockSize, s32 align, u16 flags);
REPLACE MEMHeapHandle MEMCreateUnitHeapEx(void *startAddress, u32 size, u32 blockSize, s32 align,
        u16 flags) {
    MEMHeapHandle heap = REPLACED(MEMCreateUnitHeapEx)(startAddress, size, blockSize, align, flags);
    if (!heap) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    return heap;
}

void *REPLACED(MEMAllocFromUnitHeap)(MEMHeapHandle heap);
REPLACE void *MEMAllocFromUnitHeap(MEMHeapHandle heap) {
    void *memBlock = REPLACED(MEMAllocFromUnitHeap)(heap);
    if (!memBlock) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    return memBlock;
}
