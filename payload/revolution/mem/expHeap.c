#include "expHeap.h"

MEMHeapHandle REPLACED(MEMCreateExpHeapEx)(void *startAddress, u32 size, u16 flags);
REPLACE MEMHeapHandle MEMCreateExpHeapEx(void *startAddress, u32 size, u16 flags) {
    MEMHeapHandle heap = REPLACED(MEMCreateExpHeapEx)(startAddress, size, flags);
    if (!heap) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    return heap;
}

void *REPLACED(MEMAllocFromExpHeapEx)(MEMHeapHandle heap, u32 size, int align);
REPLACE void *MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, int align) {
    void *memBlock = REPLACED(MEMAllocFromExpHeapEx)(heap, size, align);
    if (!memBlock) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    return memBlock;
}

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
