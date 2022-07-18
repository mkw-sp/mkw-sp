#pragma once

extern "C" {
#include "DecompLoader.h"
}

#include <egg/core/eggHeap.hh>

namespace SP::Storage::DecompLoader {

void Init();
bool Load(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap);
bool LoadRO(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap);
bool Load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap);
bool LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap);

} // namespace SP::Storage::DecompLoader
