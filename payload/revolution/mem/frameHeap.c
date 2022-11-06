#include "frameHeap.h"

MEMHeapHandle REPLACED(MEMCreateFrmHeapEx)(void *startAddress, u32 size, u16 flags);
REPLACE MEMHeapHandle MEMCreateFrmHeapEx(void *startAddress, u32 size, u16 flags) {
    MEMHeapHandle heap = REPLACED(MEMCreateFrmHeapEx)(startAddress, size, flags);
    if (!heap) {
        panic("Heap allocation failed!");
        __builtin_unreachable();
    }
    return heap;
}

void *REPLACED(MEMAllocFromFrmHeapEx)(MEMHeapHandle heap, u32 size, int align);
REPLACE void *MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int align) {
    void *memBlock = REPLACED(MEMAllocFromFrmHeapEx)(heap, size, align);
    if (!memBlock) {
        panic("Memory allocation failed!");
        __builtin_unreachable();
    }
    return memBlock;
}
