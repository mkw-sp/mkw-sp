#pragma once

extern "C" {
#include "DVDDecompLoader.h"
}

#include <egg/core/eggHeap.hh>

namespace SP::DVDDecompLoader {

bool load(const char *path, u8 **dst, size_t *dstSize, EGG::Heap *heap);

} // namespace SP::DVDDecompLoader
