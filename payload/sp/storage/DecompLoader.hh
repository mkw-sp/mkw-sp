#pragma once

extern "C" {
#include "DecompLoader.h"
}

#include "sp/storage/Storage.hh"

#include <egg/core/eggHeap.hh>

namespace SP::Storage::DecompLoader {

void Init();
bool Load(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap, std::optional<StorageType> storageType = {});
bool LoadRO(const char *path, size_t srcMaxSize, u64 srcOffset, u8 **dst, size_t *dstSize,
        EGG::Heap *heap, std::optional<StorageType> storageType = {});
bool Load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap,
        std::optional<StorageType> = {});
bool LoadRO(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap,
        std::optional<StorageType> = {});

} // namespace SP::Storage::DecompLoader
