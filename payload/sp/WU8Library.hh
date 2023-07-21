#pragma once

#include "sp/HeapAllocator.hh"

#include <vector>

namespace SP {

namespace WU8Library {

enum class ExtractionState {
    Started,
    Ripping,
    Decompressing,
    Processing,
    Writing,
    Finished,
    ReplacedCourse,
};

void StartExtraction(EGG::Heap *mem2);
std::tuple<ExtractionState, std::array<char, 64>> GetExtractionState();

} // namespace WU8Library

// Decodes the WU8 format inplace.
bool DecodeWU8(std::vector<u8, HeapAllocator<u8>> &wu8Buf);

} // namespace SP
