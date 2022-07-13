#pragma once

#include "egg/core/eggHeap.hh"

namespace EGG {

class DVDRipper {
public:
    enum class AllocDirection {
        Top = 1,
        Bottom = 2,
    };

    REPLACE static u8 *LoadToMainRAM(const char *path, u8 *dst, Heap *heap,
            AllocDirection allocDirection, u32 offset, u32 *readSize, u32 *fileSize);
};

} // namespace EGG
