#pragma once

#include <egg/core/eggHeap.h>

bool DecompLoader_Load(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap);
bool DecompLoader_LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap);
