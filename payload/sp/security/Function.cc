#include "Function.hh"

extern "C" {
#include <revolution/os/OSCache.h>
}

#include <cstring>

namespace SP::Function {

void KillBlacklistedFunction(u32 *startAddress, u32 *endAddress) {
    const u32 functionLength =
            reinterpret_cast<u32>(endAddress) - reinterpret_cast<u32>(startAddress);

    assert((reinterpret_cast<u32>(startAddress) & 3) == 0);
    assert((reinterpret_cast<u32>(endAddress) & 3) == 0);
    assert(startAddress < endAddress);

    memset(startAddress, 0, functionLength);
    DCFlushRange(startAddress, functionLength);
    ICInvalidateRange(startAddress, functionLength);
}

} // namespace SP::Function
