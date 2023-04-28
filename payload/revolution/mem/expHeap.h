#pragma once

#include "revolution/mem/heapCommon.h"

typedef struct MEMiExpHeapMBlockHead {
    u8 _00[0x04 - 0x00];
    u32 blockSize;
    struct MEMiExpHeapMBlockHead *mbHeadPrev;
    struct MEMiExpHeapMBlockHead *mbHeadNext;
} MEMiExpHeapMBlockHead;
static_assert(sizeof(MEMiExpHeapMBlockHead) == 0x10);

typedef struct {
    MEMiExpHeapMBlockHead *head;
    MEMiExpHeapMBlockHead *tail;
} MEMiExpMBlockList;

typedef struct {
    MEMiExpMBlockList mbFreeList;
    MEMiExpMBlockList mbUsedList;
    u8 _10[0x14 - 0x10];
} MEMiExpHeapHead;
static_assert(sizeof(MEMiExpHeapHead) == 0x14);

typedef void (*MEMHeapVisitor)(void *memBlock, MEMHeapHandle heap, u32 userParam);

void *MEMDestroyExpHeap(MEMHeapHandle heap);

void *MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, s32 align);

void MEMFreeToExpHeap(MEMHeapHandle heap, void *memBlock);

void MEMVisitAllocatedForExpHeap(MEMHeapHandle heap, MEMHeapVisitor visitor, u32 userParam);

BOOL MEMExIsAllocatedFromExpHeap(MEMHeapHandle heap, const void *memBlock);
