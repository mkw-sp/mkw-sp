#include "expHeap.h"

BOOL MEMExIsAllocatedFromExpHeap(MEMHeapHandle heap, const void *memBlock) {
    MEMiExpHeapHead *heapHead = (MEMiExpHeapHead *)(heap + 1);
    for (MEMiExpHeapMBlockHead *blockHead = heapHead->mbFreeList.head; blockHead;
            blockHead = blockHead->mbHeadNext) {
        if ((void *)blockHead <= memBlock && memBlock < (void *)((u8 *)blockHead +
                sizeof(MEMiExpHeapMBlockHead) + blockHead->blockSize)) {
            return false;
        }
    }
    return true;
}
