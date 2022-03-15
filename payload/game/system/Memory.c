#include "Memory.h"

#include <revolution/os.h>
#include <stdint.h>

void Memory_ProtectRangeModule(u32 channel, void* start, void* end, u32 permissions)
{
    assert(!(channel > OS_PROTECT_CHANNEL_3));
    assert(!(permissions & ~OS_PROTECT_PERMISSION_RW));

    u32 start_address = MemoryRoundUp1024B((uintptr_t)start);
    u32 end_address   = MemoryRoundDown1024B((uintptr_t)end);

    assert(!(end_address <= start_address));

    OSReport("[MEM] OSProtectRange: 0x%08X - 0x%08X [CHAN: %u | PERMS: %c%c]\n", start_address, end_address, channel,
                                                                                 (permissions & OS_PROTECT_PERMISSION_READ ) ? 'r' : '-',
										 (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
    OSProtectRange(channel, (void*)start_address, end_address - start_address, permissions);
}
