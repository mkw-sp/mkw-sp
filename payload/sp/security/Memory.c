#include "Memory.h"

#include <revolution/os.h>

void Memory_ProtectRange(u32 channel, void *start, void *end, u32 permissions) {
    char *startAddress = (char *)MemoryRoundUp1024B(start);
    char *endAddress = (char *)MemoryRoundDown1024B(end);
    permissions &= OS_PROTECT_PERMISSION_RW;

    assert(channel <= OS_PROTECT_CHANNEL_3);
    assert(startAddress < endAddress);

    OSReport("[MEM] OSProtectRange: %p - %p [CHAN: %u | PERMS: %c%c]\n", startAddress, endAddress,
            channel, (permissions & OS_PROTECT_PERMISSION_READ) ? 'r' : '-',
            (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
    OSProtectRange(channel, startAddress, endAddress - startAddress, permissions);
}
