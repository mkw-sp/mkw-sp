#include "eggHeap.h"

#include <revolution.h>

static void panic(void) {
    const GXColor fg = { 255, 255, 255, 255 };
    const GXColor bg = { 0, 0, 0, 255 };
    OSFatal(fg, bg, "Couldn't allocate memory!");
}

void *spAlloc(size_t size, size_t align, EGG_Heap *heap) {
    void *memBlock = EGG_Heap_alloc(size, align, heap);
    if (!memBlock) {
        panic();
    }
    return memBlock;
}

void *spAllocArray(size_t count, size_t size, size_t align, EGG_Heap *heap) {
    // TODO check for overflow
    return spAlloc(count * size, align, heap);
}

void spFree(void *memBlock) {
    EGG_Heap_free(memBlock, NULL);
}
