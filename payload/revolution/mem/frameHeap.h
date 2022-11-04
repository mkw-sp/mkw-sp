#pragma once

#include "heapCommon.h"

MEMHeapHandle MEMCreateFrmHeapEx(void *startAddress, u32 size, u16 flags);

void *MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int align);
