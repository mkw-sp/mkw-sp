#include "expHeap.h"

void *REPLACED(MEMAllocFromExpHeapEx)(MEMHeapHandle heap, u32 size, s32 align);
REPLACE void *MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, s32 align) {
    void *memBlock = REPLACED(MEMAllocFromExpHeapEx)(heap, size, align);
    if (!memBlock) {
        panic("Out of memory!");
    }
    return memBlock;
}

BOOL MEMExIsAllocatedFromExpHeap(MEMHeapHandle heap, const void *memBlock) {
    MEMiExpHeapHead *heapHead = (MEMiExpHeapHead *)(heap + 1);
    for (MEMiExpHeapMBlockHead *blockHead = heapHead->mbFreeList.head; blockHead;
            blockHead = blockHead->mbHeadNext) {
        if ((void *)blockHead <= memBlock &&
                memBlock < (void *)((u8 *)blockHead + sizeof(MEMiExpHeapMBlockHead) +
                                   blockHead->blockSize)) {
            return false;
        }
    }
    return true;
}
