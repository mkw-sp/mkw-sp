#pragma once

#include <egg/core/eggHeap.h>

bool DVDDecompLoader_Load(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap);
