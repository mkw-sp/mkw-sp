#include "DynMem.h"

typedef struct {
    EGG_ExpHeap *heapMem1;
    EGG_ExpHeap *heapMem2;
} DynMem;

static DynMem s_dynMem;

void DynMem_init(EGG_ExpHeap *heapMem1, EGG_ExpHeap *heapMem2) {
    u32 mem1Size = EGG_ExpHeap_getAllocatableSize(heapMem1, 0x4);
    s_dynMem.heapMem1 = EGG_ExpHeap_create(mem1Size, heapMem1, 0);
    u32 mem2Size = EGG_ExpHeap_getAllocatableSize(heapMem2, 0x4);
    s_dynMem.heapMem2 = EGG_ExpHeap_create(mem2Size, heapMem2, 0);
}

void DynMem_deinit(void) {
    EGG_ExpHeap_freeAll(s_dynMem.heapMem2);
    s_dynMem.heapMem2 = NULL;
    EGG_ExpHeap_freeAll(s_dynMem.heapMem1);
    s_dynMem.heapMem1 = NULL;
}

EGG_ExpHeap *DynMem_getHeapMem1(void) {
    return s_dynMem.heapMem1;
}

EGG_ExpHeap *DynMem_getHeapMem2(void) {
    return s_dynMem.heapMem2;
}
