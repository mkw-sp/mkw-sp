#include "eggHeap.h"

#include <revolution.h>

void *REPLACED(EGG_Heap_alloc)(size_t size, s32 align, EGG_Heap *heap);
REPLACE void *EGG_Heap_alloc(size_t size, s32 align, EGG_Heap *heap) {
    void *memBlock = REPLACED(EGG_Heap_alloc)(size, align, heap);
    if (!memBlock) {
        panic("Out of memory!");
        __builtin_unreachable();
    }
    if ((SP_DEBUG_LEVEL & SP_DEBUG_MEMORY_USAGE) == SP_DEBUG_MEMORY_USAGE) {
        SP_LOG("alloc %p (%zu)", memBlock, size);
    }
    return memBlock;
}

void *spAllocArray(size_t count, size_t size, s32 align, EGG_Heap *heap) {
    // TODO check for overflow
    return EGG_Heap_alloc(count * size, align, heap);
}

void spFree(void *memBlock) {
    if ((SP_DEBUG_LEVEL & SP_DEBUG_MEMORY_USAGE) == SP_DEBUG_MEMORY_USAGE) {
        SP_LOG("free %p", memBlock);
    }
    EGG_Heap_free(memBlock, NULL);
}
