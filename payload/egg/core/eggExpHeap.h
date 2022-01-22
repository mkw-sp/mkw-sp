#pragma once

#include "eggHeap.h"

typedef struct {
    EGG_Heap;
} EGG_ExpHeap;

EGG_ExpHeap *EGG_ExpHeap_create(u32 size, EGG_Heap *heap, u16 attrs);

u32 EGG_ExpHeap_getAllocatableSize(EGG_ExpHeap *this, s32 align);

void EGG_ExpHeap_freeAll(EGG_ExpHeap *this);
