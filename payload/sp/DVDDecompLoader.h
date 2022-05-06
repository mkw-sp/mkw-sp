#pragma once

#include <egg/core/eggHeap.h>

void DVDDecompLoader_init(void);

bool DVDDecompLoader_load(const char *path, u8 **dst, size_t *dstSize, EGG_Heap *heap);
