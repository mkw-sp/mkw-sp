#include "eggHeap.h"

#include <revolution.h>

void *spAlloc(size_t size, s32 align, EGG_Heap *heap) {
    void *memBlock = EGG_Heap_alloc(size, align, heap);
    if (!memBlock) {
        panic("Couldn't allocate memory!");
    }
    SP_LOG("alloc %p (%zu)", memBlock, size);
    return memBlock;
}

void *spAllocArray(size_t count, size_t size, s32 align, EGG_Heap *heap) {
    // TODO check for overflow
    return spAlloc(count * size, align, heap);
}

void spFree(void *memBlock) {
    SP_LOG("free %p", memBlock);
    EGG_Heap_free(memBlock, NULL);
}
