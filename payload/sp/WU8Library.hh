#pragma once

#include "sp/HeapAllocator.hh"

#include <vector>

namespace SP {

// Dumps all files needed for WU8 (and therefore WBZ)
// decoding to the `autoadd` folder of the SD card.
void ExtractWU8Library();
// Decodes the WU8 format inplace.
bool DecodeWU8(std::vector<u8, HeapAllocator<u8>> &wu8Buf);

} // namespace SP
