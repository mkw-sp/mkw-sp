#include "Memory.hh"

extern "C" {
#include <revolution/os.h>
}

extern "C" void Memory_InvalidateAllIBATs();

namespace SP::Memory {

#define MemoryRoundDown1024B(address) (reinterpret_cast<u32>(address) & ~0x3FF)
#define MemoryRoundUp1024B(address) ((reinterpret_cast<u32>(address) + 0x3FF) & ~0x3FF)

void InvalidateAllIBATs() {
    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(Memory_InvalidateAllIBATs);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}

void ProtectRange(u32 channel, void *start, void *end, u32 permissions) {
    char *startAddress = reinterpret_cast<char *>(MemoryRoundUp1024B(start));
    char *endAddress = reinterpret_cast<char *>(MemoryRoundDown1024B(end));
    permissions &= OS_PROTECT_PERMISSION_RW;

    assert(channel <= OS_PROTECT_CHANNEL_3);
    assert(startAddress < endAddress);

    OSReport("[MEM] OSProtectRange: %p - %p [CHAN: %u | PERMS: %c%c]\n",
            reinterpret_cast<void *>(startAddress), reinterpret_cast<void *>(endAddress), channel,
            (permissions & OS_PROTECT_PERMISSION_READ) ? 'r' : '-',
            (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
    OSProtectRange(channel, startAddress, endAddress - startAddress, permissions);
}

} // namespace SP::Memory
