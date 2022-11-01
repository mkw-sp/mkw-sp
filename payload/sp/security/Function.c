#include "Function.h"

#include <revolution/os/OSCache.h>

#include <string.h>

void Function_KillBlacklistedFunction(u32 *startAddress, u32 *endAddress) {
    const u32 functionLength = (u32)endAddress - (u32)startAddress;

    assert(((u32)startAddress & 3) == 0);
    assert(((u32)endAddress & 3) == 0);
    assert(startAddress < endAddress);

    memset(startAddress, 0, functionLength);
    DCFlushRange(startAddress, functionLength);
    ICInvalidateRange(startAddress, functionLength);
}
