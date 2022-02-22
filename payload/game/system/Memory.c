#include "Memory.h"

#include <revolution/os.h>
#include <stdint.h>

void ProtectRangeModule(unsigned int channel, void* start, void* end, unsigned int permissions)
{
    if (channel > OS_PROTECT_CHANNEL_3)
	return;

    if (permissions & ~OS_PROTECT_PERMISSION_RW)
	return;

    unsigned int start_address = MemoryRoundUp1024B((uintptr_t)start);
    unsigned int end_address   = MemoryRoundDown1024B((uintptr_t)end);

    if (end_address <= start_address)
	return;

    OSProtectRange(channel, (void*)start_address, end_address - start_address, permissions);
    OSReport("[MEM] OSProtectRange: 0x%08X - 0x%08X [CHAN: %u | PERMS: %c%c]\n", start_address, end_address, channel,
                                                                                 (permissions & OS_PROTECT_PERMISSION_READ ) ? 'r' : '-',
										 (permissions & OS_PROTECT_PERMISSION_WRITE) ? 'w' : '-');
}
