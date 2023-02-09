#include "frameHeap.h"

void *REPLACED(MEMAllocFromFrmHeapEx)(MEMHeapHandle heap, u32 size, int align);
REPLACE void *MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int align) {
    void *memBlock = REPLACED(MEMAllocFromFrmHeapEx)(heap, size, align);
    if (!memBlock) {
        panic("Out of memory!");
    }
    return memBlock;
}
