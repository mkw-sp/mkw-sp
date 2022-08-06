#include "heapCommon.h"

#include "revolution/mem/expHeap.h"

BOOL MEMExIsAllocatedFromHeap(MEMHeapHandle heap, const void *memBlock) {
    switch (heap->signature) {
    case MEMi_EXPHEAP_SIGNATURE:
        return MEMExIsAllocatedFromExpHeap(heap, memBlock);
    case MEMi_FRMHEAP_SIGNATURE:
    case MEMi_UNTHEAP_SIGNATURE:
        return true;
    default:
        return false;
    }
}
