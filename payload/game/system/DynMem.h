#pragma once

#include <egg/core/eggExpHeap.h>

void DynMem_init(EGG_ExpHeap *heapMem1, EGG_ExpHeap *heapMem2);

void DynMem_deinit(void);

EGG_ExpHeap *DynMem_getHeapMem1(void);

EGG_ExpHeap *DynMem_getHeapMem2(void);
