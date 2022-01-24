#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x38 - 0x00];
} EGG_Heap;

void *EGG_Heap_alloc(u32 size, s32 align, EGG_Heap *heap);

void EGG_Heap_free(void *memBlock, EGG_Heap *heap);

void *spAlloc(size_t size, size_t align, EGG_Heap *heap);

void *spAllocArray(size_t count, size_t size, size_t align, EGG_Heap *heap);

void spFree(void *memBlock);
