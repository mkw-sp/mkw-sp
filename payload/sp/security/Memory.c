#include "Memory.h"

#include <revolution/os.h>
#include <stdint.h>

void Memory_ProtectRangeModule(u32 channel, void* start, void* end, u32 permissions)
{
    assert(!(channel > OS_PROTECT_CHANNEL_3));
    assert(!(permissions & ~OS_PROTECT_PERMISSION_RW));

    u32* start_address = (u32*)MemoryRoundUp1024B(start);
    u32* end_address = (u32*)MemoryRoundDown1024B(end);

    assert(!(end_address <= start_address));

    OSReport("[MEM] OSProtectRange: 0x%08X - 0x%08X [CHAN: %u | PERMS: %c%c]\n", (u32)start_address, (u32)end_address, channel,
                                                                                 (permissions & OS_PROTECT_PERMISSION_READ ) ? 'r' : '-',
										 (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
    OSProtectRange(channel, start_address, (u32)end_address - (u32)start_address, permissions);
}
