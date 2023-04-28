#include "frameHeap.h"

void *REPLACED(MEMAllocFromFrmHeapEx)(MEMHeapHandle heap, u32 size, s32 align);
REPLACE void *MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, s32 align) {
    void *memBlock = REPLACED(MEMAllocFromFrmHeapEx)(heap, size, align);
    if (!memBlock) {
        panic("Out of memory!");
    }
    return memBlock;
}
