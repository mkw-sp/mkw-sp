#pragma once

#include "sp/HeapAllocator.hh"

#include <vector>

namespace SP {

namespace WU8Library {

enum class ExtractionStage {
    Started,
    Ripping,
    Decompressing,
    Processing,
    Writing,
    Finished,
    ReplacedCourse,
};

struct ExtractionState {
    ExtractionStage stage;
    std::array<char, 64> archive;
};

bool ShouldExtract();
void StartExtraction(EGG::Heap *mem2);
ExtractionState GetExtractionState();

} // namespace WU8Library

// Decodes the WU8 format inplace.
bool DecodeWU8(std::vector<u8, HeapAllocator<u8>> &wu8Buf);

} // namespace SP
