#include "Memory.h"

#include <revolution/os.h>

#define MemoryRoundDown1024B(address) (((u32)(address)) & ~0x3FF)
#define MemoryRoundUp1024B(address) (((u32)(address) + 0x3FF) & ~0x3FF)

void InvalidateAllIBATs(void);

void Memory_InvalidateAllIBATs(void) {
    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(InvalidateAllIBATs);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}

void Memory_ProtectRange(u32 channel, void *start, void *end, u32 permissions) {
    char *startAddress = (char *)MemoryRoundUp1024B(start);
    char *endAddress = (char *)MemoryRoundDown1024B(end);
    permissions &= OS_PROTECT_PERMISSION_RW;

    assert(channel <= OS_PROTECT_CHANNEL_3);
    assert(startAddress < endAddress);

    OSReport("[MEM] OSProtectRange: %p - %p [CHAN: %u | PERMS: %c%c]\n", (void *)startAddress,
            (void *)endAddress, channel, (permissions & OS_PROTECT_PERMISSION_READ) ? 'r' : '-',
            (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
    OSProtectRange(channel, startAddress, endAddress - startAddress, permissions);
}
