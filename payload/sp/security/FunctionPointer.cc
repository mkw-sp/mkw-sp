#include "FunctionPointer.hh"

extern "C" {
#include <revolution/os/OSCache.h>
}

extern "C" void FunctionPointer_CheckFunctionPointer();

namespace SP::FunctionPointer {

static u32 CreateBranchLinkInstruction(u32 *sourceAddress, u32 *destinationAddress) {
    // clang-format off
    return (18 << 26) |
           ((reinterpret_cast<u32>(destinationAddress) - reinterpret_cast<u32>(sourceAddress)) & 0x3FFFFFC) |
           (1 << 0);
    // clang-format on
}

void DoCountRegisterPatches(u32 *startAddress, u32 *endAddress) {
    const u32 bctrl = 0x4E800421;

    assert((reinterpret_cast<u32>(startAddress) & 3) == 0);
    assert((reinterpret_cast<u32>(endAddress) & 3) == 0);

    for (; startAddress < endAddress; startAddress++) {
        if (*startAddress != bctrl) {
            continue;
        }

        *startAddress = CreateBranchLinkInstruction(startAddress,
                reinterpret_cast<u32 *>(FunctionPointer_CheckFunctionPointer));

        DCFlushRange(startAddress, sizeof(*startAddress));
        ICInvalidateRange(startAddress, sizeof(*startAddress));
    }
}

} // namespace SP::FunctionPointer
