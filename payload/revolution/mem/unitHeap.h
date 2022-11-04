#pragma once

#include "heapCommon.h"

MEMHeapHandle MEMCreateUnitHeapEx(void *startAddress, u32 size, u32 blockSize, s32 align,
        u16 flags);

void *MEMAllocFromUnitHeap(MEMHeapHandle heap);
