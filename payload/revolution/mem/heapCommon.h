#pragma once

#include "revolution/mem/list.h"

enum {
    MEMi_EXPHEAP_SIGNATURE = 0x45585048,
    MEMi_FRMHEAP_SIGNATURE = 0x46524D48,
    MEMi_UNTHEAP_SIGNATURE = 0x554E5448,
};

typedef struct {
    u32 signature;
    MEMLink link;
    MEMList childList;
    u8 _04[0x3c - 0x18];
} MEMiHeapHead;
static_assert(sizeof(MEMiHeapHead) == 0x3c);

typedef MEMiHeapHead *MEMHeapHandle;

MEMHeapHandle MEMFindContainHeap(const void *memBlock);

BOOL MEMExIsAllocatedFromHeap(MEMHeapHandle heap, const void *memBlock);
