#pragma once

#include <revolution/os/OSMemory.h>

// Rounding
#define MemoryRoundDown1024B(address) (((u32)(address)) & ~0x3FF)
#define MemoryRoundUp1024B(address) (((u32)(address) + 0x3FF) & ~0x3FF)

void Memory_ProtectRangeModule(u32 channel, void* start, void* end, u32 permissions);
